#include "selectwellsbylogdialog.h"
#include "ui_selectwellsbylogdialog.h"

SelectWellsByLogDialog::SelectWellsByLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectWellsByLogDialog)
{
    ui->setupUi(this);

    connect( ui->cbLog, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells(QString)) );
}

SelectWellsByLogDialog::~SelectWellsByLogDialog()
{
    delete ui;
}


QString SelectWellsByLogDialog::log(){
    return ui->cbLog->currentText();
}

QStringList SelectWellsByLogDialog::wells(){
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    QStringList wells;
    for( auto mi : ids ){
        QString well=ui->lwWells->item(mi.row())->text();
        wells<<well;
    }
    return wells;
}

void SelectWellsByLogDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;
    updateLogs();
}


void SelectWellsByLogDialog::updateLogs(){

    ui->cbLog->clear();

    if( !m_project ) return;

    QStringList loglist;

    for( auto well : m_project->wellList()){

        for( auto log : m_project->logList(well)){
            if( !loglist.contains(log)){
                loglist.push_back(log);
            }
        }
    }

    std::sort(loglist.begin(), loglist.end());

    ui->cbLog->addItems(loglist);// loglist);
}


void SelectWellsByLogDialog::updateWells( QString log ){

    ui->lwWells->clear();

    if( !m_project) return;

    QStringList welllist;

    for( auto well : m_project->wellList() ){

        if( m_project->logList(well).contains(log)){
            welllist.push_back(well);
        }

    }

    ui->lwWells->addItems(welllist);
}

