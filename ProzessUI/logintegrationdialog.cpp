#include "logintegrationdialog.h"
#include "ui_logintegrationdialog.h"
#include<QComboBox>
#include<QPushButton>

#include<logintegrationprocess.h>
#include<iostream>

LogIntegrationDialog::LogIntegrationDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::LogIntegrationDialog)
{
    ui->setupUi(this);

    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbInput, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells()) );

    ui->cbFunction->addItems(LogIntegrationProcess::opNames());
}

LogIntegrationDialog::~LogIntegrationDialog()
{
    delete ui;
}

QMap<QString,QString> LogIntegrationDialog::params(){

    QMap<QString, QString> p;

    p.insert( "function", ui->cbFunction->currentText() );

    auto wellList=selectedWells();
    for( auto i=0; i<wellList.size(); i++){
        p.insert( tr("well-%1").arg(i+1), wellList[i] );
    }
    p.insert( "output-name", ui->leOutput->text() );
    p.insert( "unit", ui->leUnit->text());
    p.insert( "description", ui->leDescr->text());

    p.insert( "input-log", ui->cbInput->currentText() );

    return p;
}


void LogIntegrationDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->lwWells->clear();
    ui->lwWells->addItems(wells);

    updateLogs();
}

QStringList LogIntegrationDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}

void LogIntegrationDialog::updateLogs(){

    ui->cbInput->clear();

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

    ui->cbInput->addItems(loglist);
}


void LogIntegrationDialog::updateWells(){

    ui->lwWells->clear();

    if( !m_project) return;

    QStringList welllist;
    auto log=ui->cbInput->currentText();

    for( auto well : m_project->wellList() ){

        auto loglist=m_project->logList(well);
        if( loglist.contains(log) ){
            welllist.push_back(well);
        }
    }

    ui->lwWells->addItems(welllist);
}



void LogIntegrationDialog::updateOkButton(){

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
