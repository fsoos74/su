#include "smoothlogdialog.h"
#include "ui_smoothlogdialog.h"
#include<QComboBox>
#include<QPushButton>

#include<smoothprocessor.h>
#include<iostream>

SmoothLogDialog::SmoothLogDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::SmoothLogDialog)
{
    ui->setupUi(this);

    connect( ui->cbInput, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells()) );
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );

    ui->cbMethod->addItems(SmoothProcessor::opList());
}

SmoothLogDialog::~SmoothLogDialog()
{
    delete ui;
}

QMap<QString,QString> SmoothLogDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("method"), ui->cbMethod->currentText() );
    p.insert( "aperture", QString::number(ui->sbAperture->value()));
    p.insert( "input-log", ui->cbInput->currentText() );
    p.insert( "output-log", ui->leOutput->text() );
    p.insert( "unit", ui->leUnit->text());
    p.insert( "description", ui->leDescr->text());
    auto wellList=selectedWells();
    for( auto i=0; i<wellList.size(); i++){
        p.insert( tr("well-%1").arg(i+1), wellList[i] );
    }
    return p;
}


void SmoothLogDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateLogs();
}


QStringList SmoothLogDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}


void SmoothLogDialog::updateLogs(){

    ui->cbInput->clear();

    if( !m_project ) return;

    QSet<QString> logset;
    for( auto well :  m_project->wellList()){

        for( auto log : m_project->logList(well)){
            logset.insert(log);
        }
    }
    auto loglist=logset.toList();
    std::sort( loglist.begin(), loglist.end());

    ui->cbInput->addItems(loglist);
}


void SmoothLogDialog::updateWells(){

    ui->lwWells->clear();

    if( !m_project) return;

    auto log=ui->cbInput->currentText();
    QStringList welllist;

    for( auto well : m_project->wellList() ){

        auto loglist=m_project->logList(well);
        if( loglist.contains(log) ){
            welllist.push_back(well);
        }
    }

    ui->lwWells->addItems(welllist);
}


void SmoothLogDialog::updateOkButton(){

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
