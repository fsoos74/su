#include "nnlogtrainer.h"
#include "ui_nnlogtrainer.h"

#include<QPushButton>
#include<QIntValidator>
#include<QDoubleValidator>
#include<QFileDialog>
#include<QMessageBox>
#include<QInputDialog>
#include<QGraphicsScene>
#include<QGraphicsPathItem>
#include<QPainterPath>
#include<QFile>
#include "simplemlp.h"
#include "xmlpwriter.h"
#include "log.h"
#include<iostream>

NNLogTrainer::NNLogTrainer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NNLogTrainer)
{
    ui->setupUi(this);

    ui->gvError->scale(1,-1);

    auto ivalid=new QIntValidator(this);
    ivalid->setBottom(1);
    ui->leTrainingEpochs->setValidator(ivalid);

    auto dvalid=new QDoubleValidator(this);
    dvalid->setBottom(0.000001);
    dvalid->setTop(1.);
    ui->leLearningRate->setValidator(dvalid);

    auto dvalid2=new QDoubleValidator(this);
    ui->leStartMD->setValidator(dvalid2);
    ui->leStopMD->setValidator(dvalid2);

    connect(ui->lwInputs->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(invalidateNN()));
    connect(ui->sbNeurons, SIGNAL(valueChanged(int)), this, SLOT(invalidateNN()));
    connect(ui->cbWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateLogs(QString)) );
}

NNLogTrainer::~NNLogTrainer()
{
    delete ui;
}


void NNLogTrainer::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    ui->cbWell->clear();
    ui->cbWell->addItems(m_project->wellList());

    // logs will be updated via signal
}


void NNLogTrainer::getParamsFromControls(){

    m_well=ui->cbWell->currentText();
    m_predictedName=ui->cbPredicted->currentText();
    m_inputNames.clear();
    auto ids = ui->lwInputs->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        m_inputNames<<ui->lwInputs->item(idx.row())->text();
    }
    m_startMD=ui->leStartMD->text().toDouble();
    m_stopMD=ui->leStopMD->text().toDouble();
    m_hiddenNeurons=static_cast<unsigned>(ui->sbNeurons->value());
    m_trainingEpochs=ui->leTrainingEpochs->text().toUInt();
    m_learningRate=ui->leLearningRate->text().toDouble();
}

void NNLogTrainer::error(QString msg){

    QMessageBox::critical(this, "Run Training", msg, QMessageBox::Ok);
    setRunning(false);
    return void();
}

void NNLogTrainer::on_pbRun_clicked()
{

  setRunning(true);

  getParamsFromControls();  

  buildNN();

  if( m_running) prepareTraining();

  if( m_running) runTraining();

  setRunning(false);
}

void NNLogTrainer::on_pbSave_clicked()
{

   QString fn=QFileDialog::getSaveFileName(nullptr, "Save Neural Network", QDir::homePath() );
   if( fn.isEmpty()) return;

    QFile file(fn);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return error(tr("Cannot write file %1:\n%2.")
                             .arg(fn)
                             .arg(file.errorString()));
    }

    XMLPWriter xnl(m_nn, m_inputNames, 0, 0);
    if( !xnl.writeFile(&file)){
        return error("Writing NN failed!");
     }

}


void NNLogTrainer::setRunning(bool on){

    if( on==m_running) return;

    m_running=on;

    if( m_running){
        ui->teProgress->clear();
        ui->teProgress->appendPlainText("Training Started.");
        m_errors.clear();
        refreshScene();
    }else{
        ui->teProgress->appendPlainText("Training Stopped.");
    }

    ui->leLearningRate->setEnabled(!m_running);
    ui->leTrainingEpochs->setEnabled(!m_running);
    ui->cbWell->setEnabled(!m_running);
    ui->cbPredicted->setEnabled(!m_running);
    ui->leStartMD->setEnabled(!m_running);
    ui->leStopMD->setEnabled(!m_running);
    ui->lwInputs->setEnabled(!m_running);
    ui->sbNeurons->setEnabled(!m_running);
    ui->pbRun->setEnabled(!m_running);
    ui->pbStop->setEnabled(m_running);
    ui->pbSave->setEnabled(!m_running);
    ui->pbDone->setEnabled(!m_running);

    ui->pbStop->setEnabled(m_running);
}

void NNLogTrainer::setValidNN(bool on){

    if( on==m_validNN) return;

    m_validNN=on;
}

void NNLogTrainer::invalidateNN(){
    setValidNN(false);
}

void NNLogTrainer::buildNN(){
    m_nn = SimpleMLP(m_inputNames.size(),m_hiddenNeurons,m_hiddenNeurons,1);
}

void NNLogTrainer::prepareTraining(){


    auto ninputs=m_inputNames.size();

    // read input logs
    m_inputs.clear();

    for( int i=0; i<ninputs; i++){
        auto name=m_inputNames[i];
        auto l = m_project->loadLog(m_well, name);
        if( !l ){
            return error( tr("Loading log \"%1\" failed!").arg(name));
        }

        if( i>0 && l->nz()!=m_inputs[0]->nz()){
            return error( tr("Dimensions of log \"%1\" do not match first log").arg(name));
        }

        m_inputs.push_back(l);
    }

    m_predicted=m_project->loadLog(m_well,m_predictedName);
    if( !m_predicted){
        return error( tr("Loading log \"%1\" failed!").arg(m_predictedName));
    }

    if( m_inputs.empty()){
        return error("No input logs!");
    }

    auto i0=std::max(m_predicted->z2index(m_startMD),0);
    auto i1=std::min(m_predicted->z2index(m_stopMD),m_predicted->nz()-1);
    if(i1-i0<0){
        return error("No data in window!");
    }

    // build index of non null samples (all inputs and predicted)
    QVector<int> nnidx;
    //for(int i=0; i<m_predicted->nz();i++){
    for(int i=i0; i<=i1;i++){
        int nnull=0;
        if((*m_predicted)[i]==m_predicted->NULL_VALUE) nnull++;
        for( auto l : m_inputs ){
            if((*l)[i]==l->NULL_VALUE) nnull++;
        }
        if(nnull==0) nnidx.push_back(i);
    }

    // collect all data (training/testing)
    int nInputPoints = nnidx.size();
    auto allX=Matrix<double>(nInputPoints, m_inputs.size());
    auto allY=Matrix<double>(nInputPoints,1);

    for( int i=0; i<nnidx.size(); i++){
        auto idx=nnidx[i];
        allY(i,0)=(*m_predicted)[idx];
        for(int j=0; j<m_inputs.size(); j++) allX(i,j)=(*m_inputs[j])[idx];
    }

    // build training input and result matrices
    int ntraining=nInputPoints;
    if( ntraining<=0 ){
        return error("Not enought training data!!!");
    }

    // finally build training matrices
    m_X=allX;
    m_Y=allY;
/*
    // scale Y. this will be done in nn later (also save params)
    //only one column
    double omin=std::numeric_limits<double>::max();
    double omax=std::numeric_limits<double>::lowest();
    for( auto x : m_Y ){
        if(!std::isfinite(x)) continue;
        if( x<omin) omin=x;
        if( x>omax) omax=x;
    }
    if(omax>omin && omax!=0){
        for( auto& x : m_Y){
            x=(x-omin)/omax;
        }
    }
*/
    for(int i=0; i<m_X.rows(); i++){
        std::cout<<i<<" : ";
        for(int j=0; j<m_X.columns(); j++){
            std::cout<<m_X(i,j)<<"/";
        }
        std::cout<<" -> "<<m_Y(i,0)<<std::endl;
    }
}

void NNLogTrainer::setProgress(size_t epoch, double error){
    ui->teProgress->appendPlainText(tr("epoch=%1 error=%2").arg(QString::number(epoch), QString::number(error)));
    qApp->processEvents();

    m_errors.push_back(error);
    refreshScene();
}


void NNLogTrainer::runTraining(){

  setValidNN(false);
  m_nn.fit(m_X,m_Y,m_learningRate,m_trainingEpochs,0.001,
           std::bind(&NNLogTrainer::setProgress, this, std::placeholders::_1, std::placeholders::_2) );
       //    &NNVolumeTrainer::setProgress) ;
  setValidNN(true);

}

void NNLogTrainer::refreshScene(){

    auto scene=new QGraphicsScene(this);
    QPolygonF poly;
    for(int i=0; i<m_errors.size(); i++){
        poly.push_back(QPointF(i,m_errors[i]));
    }
    QPainterPath path;
    path.addPolygon(poly);
    auto item=new QGraphicsPathItem(path);
    scene->addItem(item);
    QPen pen(Qt::red, 2);
    pen.setCosmetic(true);
    item->setPen(pen);
    ui->gvError->setScene(scene);

    auto it=std::max_element(m_errors.begin(), m_errors.end());
    if( it!=m_errors.end()){
        auto maxError=*it;
        QRectF r(0,0,std::max(ui->gvError->width(),m_errors.size()), maxError);
        scene->setSceneRect(r);
        ui->gvError->fitInView(r, Qt::IgnoreAspectRatio);
    }
}

void NNLogTrainer::updateLogs(QString well){
    ui->cbPredicted->clear();
    ui->lwInputs->clear();
    if(!m_project) return;
    auto llist=m_project->logList(well);
    ui->cbPredicted->addItems(llist);
    ui->lwInputs->addItems(llist);
}

void NNLogTrainer::on_pbStop_clicked()
{
    m_nn.stop();
    setRunning(false);
}
