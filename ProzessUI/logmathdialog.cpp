#include "logmathdialog.h"
#include "ui_logmathdialog.h"
#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>

#include<mathprocessor.h>
#include<iostream>

LogMathDialog::LogMathDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::LogMathDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leValue->setValidator(validator);

    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbInput1, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells()) );
    connect( ui->cbInput2, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells()) );

    ui->cbFunction->addItems(MathProcessor::opList());
}

LogMathDialog::~LogMathDialog()
{
    delete ui;
}

QMap<QString,QString> LogMathDialog::params(){

    QMap<QString, QString> p;

    p.insert( "function", ui->cbFunction->currentText() );

    auto wellList=selectedWells();
    for( auto i=0; i<wellList.size(); i++){
        p.insert( tr("well-%1").arg(i+1), wellList[i] );
    }
    p.insert( "output-name", ui->leOutput->text() );
    p.insert( "unit", ui->leUnit->text());
    p.insert( "description", ui->leDescr->text());

    p.insert( "input-log1", ui->cbInput1->currentText() );
    p.insert( "input-log2", (ui->cbInput2->isEnabled()) ? ui->cbInput2->currentText() : "" );

    p.insert( "value", ui->leValue->text());

    return p;
}


void LogMathDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->lwWells->clear();
    ui->lwWells->addItems(wells);

    updateLogs();
}

QStringList LogMathDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}

void LogMathDialog::updateLogs(){

    ui->cbInput1->clear();
    ui->cbInput2->clear();

    if( !m_project ) return;

    QStringList loglist;

    for( auto well :  m_project->wellList()){

        for( auto log : m_project->logList(well)){
            if( !loglist.contains(log)){
                loglist.push_back(log);
            }
        }
    }

    std::sort( loglist.begin(), loglist.end());

    ui->cbInput1->clear();
    ui->cbInput1->addItems(loglist);
    ui->cbInput2->clear();
    ui->cbInput2->addItems(loglist);
}


void LogMathDialog::updateWells(){

    ui->lwWells->clear();

    if( !m_project) return;

    QStringList welllist;
    auto log1=ui->cbInput1->currentText();
    QString log2;
    if( ui->cbInput2->isEnabled() ){
        log2=ui->cbInput2->currentText();
    }

    for( auto well : m_project->wellList() ){

        auto loglist=m_project->logList(well);
        if( loglist.contains(log1) && (log2.isEmpty() || loglist.contains(log2)) ){
            welllist.push_back(well);
        }
    }

    ui->lwWells->addItems(welllist);
}



void LogMathDialog::on_cbFunction_currentIndexChanged(QString arg1)
{
    ui->leValue->setEnabled(arg1.contains("Value"));
    ui->cbInput2->setEnabled(arg1.contains("Input2"));
}


void LogMathDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    if( selectedWells().isEmpty() ) ok=false;

    auto oname=ui->leOutput->text();

    if( oname.isEmpty() ) ok=false;

    QPalette palette;
    auto wells=selectedWells();
    for( auto well : wells){
        if( m_project->existsLog(well, oname) ){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
            break;
        }
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
