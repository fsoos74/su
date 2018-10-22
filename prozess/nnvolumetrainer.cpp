#include "nnvolumetrainer.h"
#include "ui_nnvolumetrainer.h"

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
#include "simplemlp.h".h"
#include "xmlpwriter.h"


NNVolumeTrainer::NNVolumeTrainer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NNVolumeTrainer)
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

    connect(ui->lwInput->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(invalidateNN()));
    connect(ui->sbNeurons, SIGNAL(valueChanged(int)), this, SLOT(invalidateNN()));
}

NNVolumeTrainer::~NNVolumeTrainer()
{
    delete ui;
}



void NNVolumeTrainer::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    ui->cbMatching->clear();
    ui->cbMatching->addItems(m_project->tableList());

    ui->cbNonMatching->clear();
    ui->cbNonMatching->addItems(m_project->tableList());

    ui->lwInput->clear();
    ui->lwInput->addItems(m_project->volumeList());
}


void NNVolumeTrainer::getParamsFromControls(){

    m_matchingName=ui->cbMatching->currentText();
    m_nonMatchingName=ui->cbNonMatching->currentText();
    m_inputNames.clear();
    auto ids = ui->lwInput->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        m_inputNames<<ui->lwInput->item(idx.row())->text();
    }
    m_hiddenNeurons=static_cast<unsigned>(ui->sbNeurons->value());
    m_trainingEpochs=ui->leTrainingEpochs->text().toUInt();
    m_learningRate=ui->leLearningRate->text().toDouble();
}

void NNVolumeTrainer::error(QString msg){

    QMessageBox::critical(this, "Run Training", msg, QMessageBox::Ok);
    setRunning(false);
    return void();
}

void NNVolumeTrainer::on_pbRun_clicked()
{

  setRunning(true);

  getParamsFromControls();  

  buildNN();

  if( m_running) prepareTraining();

  if( m_running) runTraining();

  setRunning(false);
}

void NNVolumeTrainer::on_pbSave_clicked()
{

   QString fn=QFileDialog::getSaveFileName(nullptr, "Save Neural Network", QDir::homePath() );
   if( fn.isEmpty()) return;

    QFile file(fn);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return error(tr("Cannot write file %1:\n%2.")
                             .arg(fn)
                             .arg(file.errorString()));
    }

    XMLPWriter xnl(m_nn, m_inputNames);
    if( !xnl.writeFile(&file)){
        return error("Writing NN failed!");
     }

}


void NNVolumeTrainer::setRunning(bool on){

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
    ui->cbMatching->setEnabled(!m_running);
    ui->cbNonMatching->setEnabled(!m_running);
    ui->lwInput->setEnabled(!m_running);
    ui->sbNeurons->setEnabled(!m_running);
    ui->pbRun->setEnabled(!m_running);
    ui->pbStop->setEnabled(m_running);
    ui->pbSave->setEnabled(!m_running);
    ui->pbDone->setEnabled(!m_running);

    ui->pbStop->setEnabled(m_running);
}

void NNVolumeTrainer::setValidNN(bool on){

    if( on==m_validNN) return;

    m_validNN=on;
}

void NNVolumeTrainer::invalidateNN(){
    setValidNN(false);
}

void NNVolumeTrainer::buildNN(){
    m_nn = SimpleMLP(m_inputNames.size(),m_hiddenNeurons,m_hiddenNeurons,1);
}

void NNVolumeTrainer::prepareTraining(){

    auto ninputs=m_inputNames.size();

    // open input volumes
    m_inputVolumeReaders.clear();

    for( int i=0; i<ninputs; i++){
        auto name=m_inputNames[i];
        auto r = m_project->openVolumeReader(name);
        if( !r ){
            return error( tr("Open volume \"%1\" failed!").arg(name));
        }

        if( i > 0 ){
            if( r->bounds()!=m_inputVolumeReaders[0]->bounds()){
                return error( tr("Dimensions of volume \"%1\" do not match first volume").arg(name));
            }
        }

        m_inputVolumeReaders.push_back(r);
    }

    m_matching=m_project->loadTable(m_matchingName);
    if( !m_matching){
        return error( tr("Loading table \"%1\" failed!").arg(m_matchingName));
    }

    m_nonMatching=m_project->loadTable(m_nonMatchingName);
    if( !m_nonMatching){
        return error( tr("Loading table \"%1\" failed!").arg(m_nonMatchingName));
    }

    if( m_inputVolumeReaders.empty()){
        return error("No input volumes!");
    }

    if(m_matching->size()<1){
        return error("No matching data points!");
    }

    if(m_nonMatching->size()<1){
        return error("No non matching data points!");
    }

    // collect all data (training/testing)
    int nInputPoints = m_matching->size() + m_nonMatching->size();
    auto allX=Matrix<double>(nInputPoints, m_inputVolumeReaders.size());
    auto allY=Matrix<double>(nInputPoints,1);

    // use stupid approach for now to get started, improve later!!!
    auto bounds=m_inputVolumeReaders.front()->bounds();
    int row=0;       // row in allX/allY matrix

    // iterate over inlines
    for( int i = bounds.i1(); i<=bounds.i2(); i++){

        // load inline for all input volumes
        QVector<std::shared_ptr<Volume>> ilvols;
        for( int l = 0; l<static_cast<int>(m_inputVolumeReaders.size()); l++){
            auto ilvol = m_inputVolumeReaders[l]->readIl(i,i);
            if(!ilvol){
                return error( tr("Reading around iline=%1 from volume \"%2\" failed!").arg(QString::number(i), m_inputNames[l]));
            }
            ilvols.push_back(ilvol);
        }

        // iterate over crosslines
        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            // this cdp has no poicked points
            if( !m_matching->contains(i,j) && !m_nonMatching->contains(i,j)) continue;

            // iterate over matching points for this cdp
            auto table=m_matching->values(i,j);
            for( auto t : table){

                // iterate over volumes
                for( int vi=0; vi<ilvols.size(); vi++){
                    auto col=vi;
                    allX(row,col)=ilvols[vi]->value(i,j,t);
                }
                allY( row, 0)=1.;            // matching == 1
                row++;                       // next row
            }


            // iterate over non matching points for this cdp
            table=m_nonMatching->values(i,j);
            for( auto t : table){

                // iterate over volumes
                for( int vi=0; vi<ilvols.size(); vi++){
                    auto col=vi;
                    allX(row,col)=ilvols[vi]->value(i,j,t);
                }
                allY( row, 0)=0.;            // non matching == 0
                row++;                       // next row
            }
        }
    }

    // build training input and result matrices
    int ntraining=row;
    if( ntraining<=0 ){
        return error("Not enought training data!!!");
    }

    // finally build training matrices
    m_X=allX;
    m_Y=allY;
}

void NNVolumeTrainer::setProgress(size_t epoch, double error){
    ui->teProgress->appendPlainText(tr("epoch=%1 error=%2").arg(QString::number(epoch), QString::number(error)));
    qApp->processEvents();

    m_errors.push_back(error);
    refreshScene();
}


void NNVolumeTrainer::runTraining(){

  setValidNN(false);
  m_nn.fit(m_X,m_Y,m_learningRate,m_trainingEpochs,std::bind(&NNVolumeTrainer::setProgress, this, std::placeholders::_1, std::placeholders::_2) );
       //    &NNVolumeTrainer::setProgress) ;
  setValidNN(true);

}

void NNVolumeTrainer::refreshScene(){

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

void NNVolumeTrainer::on_pbStop_clicked()
{
    m_nn.stop();
    setRunning(false);
}
