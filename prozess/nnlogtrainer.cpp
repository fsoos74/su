#include "nnlogtrainer.h"
#include "ui_nnlogtrainer.h"

#include<QPushButton>
#include<QIntValidator>
#include<QDoubleValidator>
#include<QFileDialog>
#include<QMessageBox>
#include<QInputDialog>

#include<QFile>
#include "nn.h"
#include "nnfunc.h"
#include "xnlwriter.h"
#include<log.h>
#include<smoothprocessor.h>


void filterLog(Log& log, int aperture){

    auto tmp=log;
    SmoothProcessor processor;
    processor.setInputNullValue(log.NULL_VALUE);
    processor.setOP(SmoothProcessor::OP::MEDIAN);
    int hw=aperture/2;

    for( int i=0; i<log.nz(); i++){

        processor.clearInput();

        int jmin=std::max( 0, i-hw);
        int jmax=std::min( log.nz()-1, i+hw);
        for( int j=jmin; j<=jmax; j++){
            auto value=tmp[j];
            auto dist=std::abs(i-j);
            processor.addInput(value, dist);
        }

        auto res=processor.compute();

        if( res!=processor.NULL_VALUE) log[i]=res;

    }
}




NNLogTrainer::NNLogTrainer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NNLogTrainer)
{
    ui->setupUi(this);

    auto ivalid=new QIntValidator(this);
    ivalid->setBottom(1);
    ui->leTrainingEpochs->setValidator(ivalid);

    auto dvalid=new QDoubleValidator(this);
    dvalid->setBottom(0.000001);
    dvalid->setTop(1.);
    ui->leTrainingRatio->setValidator(dvalid);
    ui->leLearningRate->setValidator(dvalid);

    connect( ui->cbWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInputAndPredicted(QString)) );
    connect(ui->cbWell, SIGNAL(currentTextChanged(QString)), this, SLOT(invalidateNN()));
    connect(ui->lwInput->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(invalidateNN()));
    connect(ui->sbNeurons, SIGNAL(valueChanged(int)), this, SLOT(invalidateNN()));
}

NNLogTrainer::~NNLogTrainer()
{
    delete ui;
}



void NNLogTrainer::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->cbWell->clear();
    ui->cbWell->addItems(wells);
}


void NNLogTrainer::updateInputAndPredicted(QString well){

    if( !m_project ) return;

    ui->lwInput->clear();
    ui->lwInput->addItems(m_project->logList(well));

    ui->cbPredicted->clear();
    ui->cbPredicted->addItems(m_project->logList(well));
}


void NNLogTrainer::getParamsFromControls(){

    m_well = ui->cbWell->currentText();
    m_predictedName = ui->cbPredicted->currentText();
    m_inputNames.clear();
    auto ids = ui->lwInput->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        m_inputNames<<ui->lwInput->item(idx.row())->text();
    }
    m_hiddenNeurons=static_cast<unsigned>(ui->sbNeurons->value());
    m_trainingEpochs=ui->leTrainingEpochs->text().toUInt();
    m_trainingRatio=ui->leTrainingRatio->text().toDouble();
    m_learningRate=ui->leLearningRate->text().toDouble();
    m_maxNoDecrease=static_cast<unsigned>(ui->sbIncreasingEpochs->value());
    m_filterlen=ui->sbMedianFilter->value();
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

  if( m_running) buildNN();

  if( m_running) prepareTraining();

  if( m_running) runTraining();

  setRunning(false);
}

void NNLogTrainer::on_pbSave_clicked()
{

   QString fn=QFileDialog::getSaveFileName(nullptr, "Save Neurale Network", QDir::homePath() );
   if( fn.isEmpty()) return;

    QFile file(fn);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return error(tr("Cannot write file %1:\n%2.")
                             .arg(fn)
                             .arg(file.errorString()));
    }

    XNLWriter xnl(m_nn, m_inputNames, m_Xmm, m_Ymm);
    if( !xnl.writeFile(&file)){
        return error("Writing NN failed!");
     }
}

void NNLogTrainer::on_pbTest_clicked()
{
    prepareTest();
    runTest();
}



void NNLogTrainer::setRunning(bool on){

    if( on==m_running) return;

    m_running=on;

    if( m_running){
        ui->teProgress->clear();
        ui->teProgress->appendPlainText("Training Started.");
    }else{
        ui->teProgress->appendPlainText("Training Stopped.");
    }

    ui->leLearningRate->setEnabled(!m_running);
    ui->leTrainingEpochs->setEnabled(!m_running);
    ui->leTrainingRatio->setEnabled(!m_running);
    ui->cbPredicted->setEnabled(!m_running);
    ui->cbWell->setEnabled(!m_running);
    ui->lwInput->setEnabled(!m_running);
    ui->sbNeurons->setEnabled(!m_running);
    ui->sbMedianFilter->setEnabled(!m_running);
    ui->pbRun->setEnabled(!m_running);
    ui->pbSave->setEnabled(!m_running);
    ui->pbDone->setEnabled(!m_running);

    ui->pbStop->setEnabled(m_running);
}

void NNLogTrainer::setValidNN(bool on){

    if( on==m_validNN) return;

    m_validNN=on;

    ui->pbTest->setEnabled(m_validNN);
}

void NNLogTrainer::invalidateNN(){
    setValidNN(false);
}

void NNLogTrainer::on_pbStop_clicked()
{
    setRunning(false);
}

void NNLogTrainer::buildNN(){
    //create NN
    NN nn{ static_cast<size_t>(m_inputNames.size()), m_hiddenNeurons, 1};      // XXX

    nn.setSigma(leaky_relu);
    nn.setSigmaPrime(leaky_relu_prime);
    nn.setEta(m_learningRate);

    m_nn = nn;

}

void NNLogTrainer::prepareTraining(){

    auto ninputs=m_inputNames.size();

    // load input
    m_inputLogs = std::vector<std::shared_ptr<Log>>(ninputs);

    for( int i=0; i<ninputs; i++){
        auto name=m_inputNames[i];
        auto l = m_project->loadLog(m_well, name);
        if( !l ){
            return error( tr("Loading log %1-%2 failed!").arg(m_well,name));
        }
        m_inputLogs[i]=l;
    }

    std::shared_ptr<Log> predictedLog=m_project->loadLog(m_well, m_predictedName);
    if( !predictedLog ){
        return error( tr("Loading log %1-%2 failed!").arg(m_well,m_predictedName));
    }

    // filter logs if required
    if( m_filterlen>1){
        for(int i=0; i<ninputs; i++){
            filterLog(*m_inputLogs[i], m_filterlen);
        }
        filterLog(*predictedLog, m_filterlen);
    }

    // prepare training input and results

    // make list of non null input and result samples for training
    std::vector<int> s_good_training;
    for( int i=0; i<predictedLog->nz(); i++){
        size_t good=0;
        for( auto l : m_inputLogs){
            if( (*l)[i]!=l->NULL_VALUE ) good++;
        }
        if( good<m_inputLogs.size()) continue;                                // at least one log has NULL value for this sample, skip
        if( (*predictedLog)[i]==predictedLog->NULL_VALUE ) continue;        // predicted log has NULL value for this sample, skip
        s_good_training.push_back(i);                                       // finally found a usuable sample, store it
    }

    // build training input and result matrices
    // randomly select training ratio good samples
    int ntraining=static_cast<int>( std::round(m_trainingRatio * s_good_training.size()));
    if( ntraining<=0 ){
        return error("Not enought training data!!!");
    }

    random_shuffle( s_good_training.begin(), s_good_training.end());

    // finally build training matrices
    m_X=Matrix<double>(ntraining, m_inputLogs.size());
    m_Y=Matrix<double>(ntraining,1);
    Matrix<double>& X=m_X;
    Matrix<double>& Y=m_Y;

    for( int it=0; it<ntraining;it++){

        auto i=s_good_training[it];

        std::cout<<"it="<<it<<" i="<<i<<" [ ";
        for( size_t j=0; j<m_inputLogs.size(); j++){
            auto x=(*m_inputLogs[j])[i];
            X(it,j)=x;
            std::cout<<x<<" ";
        }
        std::cout<<"] -> ";
        auto y=(*predictedLog)[i];
        Y(it,0)=y;
        std::cout<<y<<std::endl;
    }


    // determine normalization params on training data, training data has no NULLs
   m_Xmm=std::vector< std::pair<double,double> >(X.columns());
   for( unsigned j = 0; j<X.columns(); j++){
       auto itmm = std::minmax_element( X.column_begin(j), X.column_end(j));
       m_Xmm[j]=std::make_pair( *itmm.first, *itmm.second );
   }
   auto itmm=std::minmax_element( Y.begin(), Y.end() );
   m_Ymm=std::make_pair(*itmm.first, *itmm.second );

   // normalize training data
   for( unsigned j=0; j<X.columns(); j++){
       norm( X.column_begin(j), X.column_end(j), m_Xmm[j], m_inputLogs[j]->NULL_VALUE);
   }
   norm( Y.begin(), Y.end(), m_Ymm, predictedLog->NULL_VALUE );

}

void NNLogTrainer::runTraining(){

    setValidNN(false);

    const Matrix<double>& X=m_X;
    const Matrix<double>& Y=m_Y;

  // split training data into input and test data
  std::vector<size_t> indices;
  indices.reserve(X.rows());
  for( size_t i=0; i<X.rows(); i++){
      indices.push_back(i);
  }
  std::random_shuffle(indices.begin(), indices.end());

  size_t ntrain=std::min(X.rows(),static_cast<size_t>( m_trainingRatio*X.rows()) );
  size_t ntest=X.rows()-ntrain;

  // extract training data
  Matrix<double> Xtrain(ntrain, X.columns());
  Matrix<double> Ytrain(ntrain, Y.columns());
  for( size_t i=0; i<Xtrain.rows(); i++){
      auto irow=indices[i];
      for( size_t j=0; j<X.columns(); j++){
          Xtrain(i,j)=X(irow,j);
      }
      for( size_t j=0; j<Y.columns(); j++){
          Ytrain(i,j)=Y(irow,j);
      }
  }

  // extract test data
  Matrix<double> Xtest(ntest, X.columns());
  Matrix<double> Ytest(ntest, Y.columns());
  for( size_t i=0; i<Xtest.rows(); i++){
      auto irow=indices[i+ntrain];
      for( size_t j=0; j<X.columns(); j++){
          Xtest(i,j)=X(irow,j);
      }
      for( size_t j=0; j<Y.columns(); j++){
          Ytest(i,j)=Y(irow,j);
      }
  }

  double last_error=std::numeric_limits<double>::max();
  double err=last_error;
  size_t epoch=0;
  size_t n_inc_error=0;

  while( m_running && (epoch<m_trainingEpochs) ){

      try{
      m_nn.train(Xtrain,Ytrain,1);
      }catch(std::exception& ex){
          error(QString(ex.what()));
      }

      last_error=err;
      err=average_error(m_nn, Xtest,Ytest);
      epoch++;
      std::cout<<"epoch: "<<epoch<<" err: "<<err<<std::endl<<std::flush;
      if( err>=last_error ){
          n_inc_error++;
          if( n_inc_error>m_maxNoDecrease ) break;
      }
      else{
          n_inc_error=0;
      }

      //progress(epoch,err);
      ui->teProgress->appendPlainText(tr("epoch=%1 error=%2").arg(QString::number(epoch+1), QString::number(err)));
      qApp->processEvents();

  }

  setValidNN(true);

}



void NNLogTrainer::prepareTest(){
    QString name;
    while(1){
        bool ok=false;
        name=QInputDialog::getText(nullptr, "Test Run Neural Network", "Enter Output Log Name:", QLineEdit::Normal, name, &ok);
        if( !ok || name.isNull()) return;
        if( m_project->logList(m_well).contains(name)){
                QMessageBox::critical(this, "TestRun Neural Network", tr("Well \"%1\" already contains log \"%2\"").arg(m_well,name), QMessageBox::Ok);
                continue;
        }
        break;
    }

    m_outputLogName=name;

    const Log& ref=*m_inputLogs.front();
    m_outputLog=std::make_shared<Log>( m_outputLogName.toStdString(), "","",//m_unit.toStdString(), m_descr.toStdString(),
                                       ref.z0(), ref.dz(), ref.nz() );

    if( !m_outputLog){
        return error("Allocating log failed!");
    }
}


// assumes that training was run before, validNN
void NNLogTrainer::runTest(){

    // normalizing of inputs was done furing training
    Matrix<double> x(1, m_inputLogs.size());  // prozess sample by sample
    Matrix<double> y( 1, 1);

    for( size_t i=0; i<static_cast<size_t>(m_outputLog->nz()); i++){

        bool ok=true;
        for( size_t j=0; j<m_inputLogs.size(); j++){
            auto v=(*(m_inputLogs[j]))[i];
            if( v==m_inputLogs[j]->NULL_VALUE ){
                ok=false;
                break;
            }
            x(0,j)=v;
            norm(x.column_begin(j), x.column_end(j), m_Xmm[j], m_inputLogs[j]->NULL_VALUE);
        }

        y = m_nn.feed_forward(x);

        if(ok) (*m_outputLog)[i]=y(0,0);
    }

    unnorm( m_outputLog->begin(), m_outputLog->end(), m_Ymm, m_outputLog->NULL_VALUE);

    if( !m_project->addLog(m_well, m_outputLogName, *m_outputLog)){
        return error("Adding Log to project failed!");
    }
}

/*
Log medianFilter( const Log& log, int len ){

    Log res( "median filtered", log.unit(), "median filtered", log.z0(), log.dz(), log.nz() );

    for( int i=0; i<log.nz(); i++ ){

        int start = std::max( 0, i-len/2);
        int stop = std::min(log.nz()-1, i + len/2 );
        std::vector<double> buf;
        buf.reserve(len);
        for( int i=start; i<=stop; i++){
            if( log[i]==log.NULL_VALUE) continue;
            buf.push_back(log[i]);
        }

        if( buf.size()>0 ){
            std::nth_element( &buf[0], &buf[buf.size()/2], &buf[buf.size()-1] );
            res[i]=buf[buf.size()/2];
        }
        else{
            res[i]=res.NULL_VALUE;
        }
    }

    return res;
}
*/
