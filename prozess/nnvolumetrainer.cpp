#include "nnvolumetrainer.h"
#include "ui_nnvolumetrainer.h"

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



NNVolumeTrainer::NNVolumeTrainer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NNVolumeTrainer)
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
    m_miniBatchSize=ui->leTrainingRatio->text().toDouble();
    m_learningRate=ui->leLearningRate->text().toDouble();
    m_maxNoDecrease=static_cast<unsigned>(ui->sbIncreasingEpochs->value());
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

  if( m_running) buildNN();

  if( m_running) prepareTraining();

  if( m_running) runTraining();

  setRunning(false);
}

void NNVolumeTrainer::on_pbSave_clicked()
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


void NNVolumeTrainer::setRunning(bool on){

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
    ui->cbMatching->setEnabled(!m_running);
    ui->cbNonMatching->setEnabled(!m_running);
    ui->lwInput->setEnabled(!m_running);
    ui->sbNeurons->setEnabled(!m_running);
    ui->pbRun->setEnabled(!m_running);
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

void NNVolumeTrainer::on_pbStop_clicked()
{
    setRunning(false);
}

void NNVolumeTrainer::buildNN(){
    //create NN
    NN nn{ static_cast<size_t>(m_inputNames.size()*NIL*NXL), m_hiddenNeurons, 1};

    nn.setSigma(leaky_relu);
    nn.setSigmaPrime(leaky_relu_prime);
    nn.setEta(m_learningRate);

    m_nn = nn;

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
    auto allX=Matrix<double>(nInputPoints, m_inputVolumeReaders.size()*NIL*NXL );
    auto allY=Matrix<double>(nInputPoints,1);

    // use stupid approach for now to get started, improve later!!!
    auto bounds=m_inputVolumeReaders.front()->bounds();
    int row=0;       // row in allX/allY matrix

    // iterate over inlines
    for( int i = bounds.i1()+NIL/2; i<=bounds.i2()-NIL/2; i++){

        // load inline for all input volumes
        QVector<std::shared_ptr<Volume>> ilvols;
        for( int l = 0; l<static_cast<int>(m_inputVolumeReaders.size()); l++){
            auto ilvol = m_inputVolumeReaders[l]->readIl(i-NIL/2,i+NIL/2);
            if(!ilvol){
                return error( tr("Reading around iline=%1 from volume \"%2\" failed!").arg(QString::number(i), m_inputNames[l]));
            }
            ilvols.push_back(ilvol);
        }

        // iterate over crosslines
        for( int j=bounds.j1()+NXL/2; j<=bounds.j2()-NXL/2; j++){

            if( !m_matching->contains(i,j) && !m_nonMatching->contains(i,j)) continue;

            // iterate over matching points for this cdp
            auto table=m_matching->values(i,j);
            for( auto t : table){

                // iterate over il aperture
                for( int ii=0; ii<NIL; ii++){
                    //iterate over xl aperture
                    for( int jj=0; jj<NXL; jj++){
                        // iterate over volumes
                        for( int vi=0; vi<ilvols.size(); vi++){
                            auto col=(ii*NXL + jj)*ilvols.size()+vi;
                            allX(row,col)=ilvols[vi]->value(i+ii-NIL/2,j+jj-NXL/2,t);
                        }
                    }
                }

                allY( row, 0)=1.;            // matching == 1

                row++;                       // next row
            }


            // iterate over non matching points for this cdp
            table=m_nonMatching->values(i,j);
            for( auto t : table){

                // iterate over il aperture
                for( int ii=0; ii<NIL; ii++){
                    //iterate over xl aperture
                    for( int jj=0; jj<NXL; jj++){
                        // iterate over volumes
                        for( int vi=0; vi<ilvols.size(); vi++){
                            auto col=(ii*NXL + jj)*ilvols.size()+vi;
                            allX(row,col)=ilvols[vi]->value(i+ii-NIL/2,j+jj-NXL/2,t);
                        }
                    }
                }

                allY( row, 0)=0.;            // non matching == 0

                row++;                       // next row
            }
        }
    }

/*
    // testing qc
    std::cout<<"ALLX: rows="<<allX.rows()<<" COLS="<<allX.columns()<<std::endl<<std::flush;
    for( int i=0; i<allX.rows(); i++){
        std::cout<<"i="<<i<<std::endl<<std::flush;
        std::cout<<"( ";
        for( int j=0; j<allX.columns(); j++){
            std::cout<<allX(i,j)<<", ";
        }
        std::cout<<") -> "<<allY(i,0)<<std::endl;
    }
    std::cout<<std::endl<<std::flush;
    return;
*/
    // build training input and result matrices
    int ntraining=row;
    if( ntraining<=0 ){
        return error("Not enought training data!!!");
    }

    // finally build training matrices
    m_X=allX;
    m_Y=allY;
    Matrix<double>& X=m_X;
    Matrix<double>& Y=m_Y;

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
       norm( X.column_begin(j), X.column_end(j), m_Xmm[j]);
   }
   norm( Y.begin(), Y.end(), m_Ymm );

}

// stochastic gradient descent
void NNVolumeTrainer::runTraining(){

  setValidNN(false);

  Matrix<double> XMini(m_miniBatchSize, m_X.columns());
  Matrix<double> YMini(m_miniBatchSize, m_Y.columns());
  std::vector<size_t> indices;
  indices.reserve(m_X.rows());
  for( size_t i=0; i<m_X.rows(); i++){
      indices.push_back(i);
  }

  double last_err=std::numeric_limits<double>::max();
  double err=last_err;
  size_t epoch=0;
  size_t n_increasing=0;

  while( m_running && (epoch<m_trainingEpochs) ){

      std::random_shuffle( indices.begin(), indices.end());
      for( int i=0; i<m_miniBatchSize; i++){
          auto idx=indices[i];
          for(int j=0; j<m_X.columns(); j++) XMini(i,j)=m_X(idx,j);
          for(int j=0; j<m_Y.columns(); j++) YMini(i,j)=m_Y(idx,j);
      }
      NN last_nn=m_nn;
      m_nn.train(XMini,YMini,1);
      auto err=average_error(m_nn,m_X,m_Y);

      if(err>last_err){
          if( n_increasing==0){
              m_nn=last_nn;
          }
          n_increasing++;
          if( n_increasing==m_maxNoDecrease){
              m_nn=last_nn;
              break;
          }
      }
      else{
          n_increasing=0;
      }
      last_err=err;

      ui->teProgress->appendPlainText(tr("epoch=%1 error=%2").arg(QString::number(epoch+1), QString::number(err)));
      qApp->processEvents();

      epoch++;
  }

  setValidNN(true);

}


