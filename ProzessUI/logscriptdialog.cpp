#include "logscriptdialog.h"
#include "ui_logscriptdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QPushButton>
#include<avoproject.h>

LogScriptDialog::LogScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogScriptDialog)
{
    ui->setupUi(this);

    // set monospace font for code editing
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPixelSize(14);
    ui->teScript->setFont(font);


    inputCB.resize(ui->sbInputLogs->maximum()); // fills with nullptr

    connect( ui->sbInputLogs, SIGNAL(valueChanged(int)),
             this, SLOT(updateInputLogControls()));

    connect( ui->leLog, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );
    connect( ui->teScript, SIGNAL(textChanged()), this, SLOT(updateOkButton()) );

    //updateLogs();
    //updateInputLogControls();

    updateOkButton();
}

LogScriptDialog::~LogScriptDialog()
{
    delete ui;
}


void LogScriptDialog::setProject(AVOProject* p){

    if( p==m_project) return;
    m_project=p;
    updateInputLogControls();
    //updateInputLogs();
   /*
    QString well;
    ui->lwWells->clear();

    if( m_project ){
        auto wells=m_project->wellList();
        ui->lwWells->addItems(wells);
    }
    */
}


QStringList LogScriptDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}

void LogScriptDialog::updateLogs(){

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

    for( int i=0; i<ui->sbInputLogs->value(); i++){
        QComboBox* cb=inputCB[i];
        if( cb ){
            QString old=cb->currentText();
            cb->clear();
            cb->addItems(loglist);
            if( !old.isEmpty() && loglist.contains(old)) cb->setCurrentText(old);
        }
    }
}


void LogScriptDialog::updateWells(){

    ui->lwWells->clear();

    if( !m_project) return;

    QStringList welllist;

    for( auto well : m_project->wellList() ){

        auto loglist=m_project->logList(well);
        int n=0;
        for( int i=0; i<ui->sbInputLogs->value(); i++){
            QComboBox* cb=inputCB[i];
            if( cb && loglist.contains(cb->currentText())) n++;
        }

        if( n==ui->sbInputLogs->value()){   // this well contains all input logs
            welllist.push_back(well);
        }
    }

    ui->lwWells->addItems(welllist);
}



QMap<QString,QString> LogScriptDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("log"), ui->leLog->text() );
    p.insert( "unit", ui->leUnit->text());
    p.insert("description", ui->leDescr->text());

    p.insert( QString("n_input_logs"), QString::number(ui->sbInputLogs->value()));
    for( int i=0; i<ui->sbInputLogs->value(); i++){
        p.insert( QString("input_log_")+QString::number(i+1), inputCB[i]->currentText() );
    }

    auto wellList=selectedWells();
    for( auto i=0; i<wellList.size(); i++){
        p.insert( tr("well-%1").arg(i+1), wellList[i] );
    }

    p.insert( QString("script"), ui->teScript->toPlainText() );

    return p;
}

void LogScriptDialog::updateInputLogControls(){

    if( ui->wdInputLogs->layout()){
        delete ui->wdInputLogs->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputLogs);
    for( int i=0; i<ui->sbInputLogs->value(); i++){

        if( !inputCB[i]){
            QComboBox* cb=new QComboBox( this );
            inputCB[i]=cb;
            connect( cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells()) );
        }

        layout->addWidget(inputCB[i], i, 1);
    }

    ui->wdInputLogs->setLayout(layout);
    ui->wdInputLogs->updateGeometry();

    updateLogs();
}

void LogScriptDialog::updateOkButton(){

    if( !m_project ) return;

    bool ok=true;

    if( ui->teScript->toPlainText().isEmpty()) ok=false;

    if( ui->leLog->text().isEmpty()){
        ok=false;
    }

    auto wells=selectedWells();
    if( wells.isEmpty()) ok=false;

    QPalette palette;
    for( auto well : wells ){
        if( m_project->existsLog( well, ui->leLog->text())){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
            break;
        }
    }
    ui->leLog->setPalette(palette);

    ui->pbOK->setEnabled(ok);
}



void LogScriptDialog::on_pbLoad_clicked()
{
    QString fn=QFileDialog::getOpenFileName(this,
        "Open Script", QDir::homePath(), "*.js");

    if( fn.isNull()) return;

    QFile theFile(fn);
    theFile.open(QIODevice::ReadOnly);
    // add error checking code HERE

    QTextStream textStream(&theFile);
    ui->teScript->setPlainText(textStream.readAll());
    theFile.close();
}

void LogScriptDialog::on_pbSave_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this,
        "Save Script", QDir::homePath(), "*.js");


    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            // error message
        } else {
            QTextStream stream(&file);
            stream << ui->teScript->toPlainText();
            stream.flush();
            file.close();
        }
    }
}
