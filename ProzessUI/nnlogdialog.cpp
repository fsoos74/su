#include "nnlogdialog.h"
#include "ui_nnlogdialog.h"

#include<QPushButton>
#include<QIntValidator>
#include<QDoubleValidator>

#include<QFileDialog>

NNLogDialog::NNLogDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::NNLogDialog)
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
    connect( ui->lwInput ->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );
    connect( ui->cbPredicted, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbParam, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()) );
    connect( ui->leParamFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbHist, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()) );
    connect( ui->leHist, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

NNLogDialog::~NNLogDialog()
{
    delete ui;
}


QMap<QString,QString> NNLogDialog::params(){

    QMap<QString, QString> p;

    p.insert( "well", ui->cbWell->currentText() );

    auto logList=selectedLogs();
    for( auto i=0; i<logList.size(); i++){
        p.insert( tr("input-log-%1").arg(i+1), logList[i] );
    }

    p.insert( "predicted-log", ui->cbPredicted->currentText() );
    p.insert( "output-log", ui->leOutput->text());

    //p.insert( "unit", ui->leUnit->text());
    //p.insert( "description", ui->leDescr->text());

    p.insert( "hidden-neurons", QString::number(ui->sbNeurons->value()));

    p.insert("training-ratio", ui->leTrainingRatio->text());
    p.insert("training-epochs", ui->leTrainingEpochs->text());
    p.insert("learning-rate", ui->leLearningRate->text());

    QString pfile;
    if( ui->cbParam->isChecked()) pfile=ui->leParamFile->text();
    p.insert("param-file", pfile);

    QString hfile;
    if( ui->cbHist->isChecked()) hfile=ui->leHist->text();
    p.insert("history-file", hfile);

    return p;
}



void NNLogDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->cbWell->clear();
    ui->cbWell->addItems(wells);
}


QStringList NNLogDialog::selectedLogs(){

    QStringList logs;
    auto ids = ui->lwInput->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        logs<<ui->lwInput->item(idx.row())->text();
    }
    return logs;
}

void NNLogDialog::updateInputAndPredicted(QString well){

    if( !m_project ) return;

    ui->lwInput->clear();
    ui->lwInput->addItems(m_project->logList(well));

    ui->cbPredicted->clear();
    ui->cbPredicted->addItems(m_project->logList(well));
}

void NNLogDialog::on_pbParamBrowse_clicked(){

    QString filter = "NN Log Files (*.xnl)";

    QFileDialog dialog(this, tr("Save NN Log Parameters"), QDir::homePath(), filter);
    dialog.setDefaultSuffix(tr(".xnl"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if( dialog.exec()==QDialog::Accepted && !dialog.selectedFiles().empty()){

        QString fn=dialog.selectedFiles().front();

       if( !fn.isNull()){
            ui->leParamFile->setText(fn);
            updateOkButton();
        }
    }
}


void NNLogDialog::on_pbHistBrowse_clicked(){

    QString filter = "Text File (*.txt)";

    QFileDialog dialog(this, tr("Save Training History "), QDir::homePath(), filter);
    dialog.setDefaultSuffix(tr(".txt"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if( dialog.exec()==QDialog::Accepted && !dialog.selectedFiles().empty()){

        QString fn=dialog.selectedFiles().front();

       if( !fn.isNull()){
            ui->leHist->setText(fn);
            updateOkButton();
        }
    }
}


void NNLogDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    auto inputs=selectedLogs();
    auto predicted=ui->cbPredicted->currentText();
    auto oname=ui->leOutput->text();

     if( inputs.isEmpty() ) ok=false;
/*
    {
    QPalette palette;
    if( inputs.contains(predicted) ){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
    }
    ui->cbPredicted->setPalette(palette);
    }
*/
    {
    QPalette palette;
    if( oname.isEmpty() || inputs.contains(oname) ){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);
    }

     if( ui->cbParam->isChecked() && ui->leParamFile->text().isEmpty()){
         ok=false;
     }

    if( ui->cbHist->isChecked() && ui->leHist->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
