#include "runvolumescriptdialog.h"
#include "ui_runvolumescriptdialog.h"


#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QPushButton>
#include<avoproject.h>

RunVolumeScriptDialog::RunVolumeScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunVolumeScriptDialog)
{
    ui->setupUi(this);

    // set monospace font for code editing
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPixelSize(14);
    ui->teScript->setFont(font);


    inputVolumeCB.resize(ui->sbInputGrids->maximum()); // fills with nullptr
    inputGridCB.resize(ui->sbInputGrids->maximum()); // fills with nullptr

    connect( ui->sbInputVolumes, SIGNAL(valueChanged(int)),
             this, SLOT(updateInputVolumeControls()));
    connect( ui->sbInputGrids, SIGNAL(valueChanged(int)),
             this, SLOT(updateInputGridControls()));

    connect( ui->leResultVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect( ui->teScript, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );


    updateInputVolumeControls(); // create first lineedit

    updateOkButton();
}

RunVolumeScriptDialog::~RunVolumeScriptDialog()
{
    delete ui;
}

void RunVolumeScriptDialog::setInputVolumes( const QStringList& l){

    if( l==m_inputVolumes) return;

    m_inputVolumes=l;

    for( QComboBox* cb : inputVolumeCB){
        if( cb ){
            cb->clear();
            cb->addItems(m_inputVolumes);
        }
    }

    updateOkButton();
}

void RunVolumeScriptDialog::setInputGrids( const QStringList& l){

    if( l==m_inputGrids) return;

    m_inputGrids=l;

    for( QComboBox* cb : inputGridCB){
        if( cb ){
            cb->clear();
            cb->addItems(m_inputGrids);
        }
    }

    updateOkButton();
}

QMap<QString,QString> RunVolumeScriptDialog::params(){

    QMap<QString, QString> p;



    p.insert( QString("result"), ui->leResultVolume->text());

    p.insert( QString("n_input_volumes"), QString::number(ui->sbInputVolumes->value()));
    for( int i=0; i<ui->sbInputVolumes->value(); i++){
        p.insert( QString("input_volume_")+QString::number(i+1),
                inputVolumeCB[i]->currentText() );
    }

    p.insert( QString("n_input_grids"), QString::number(ui->sbInputGrids->value()));
    for( int i=0; i<ui->sbInputGrids->value(); i++){
        p.insert( QString("input_grid_")+QString::number(i+1),
                inputGridCB[i]->currentText() );
    }


    p.insert( QString("script"), ui->teScript->toPlainText() );

    return p;
}

void RunVolumeScriptDialog::updateInputVolumeControls(){

    if( ui->wdInputVolumes->layout()){
        delete ui->wdInputVolumes->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputVolumes);
    for( int i=0; i<ui->sbInputVolumes->value(); i++){

        if( !inputVolumeCB[i]){
            QComboBox* cb=new QComboBox( this );
            cb->clear();
            cb->addItems(m_inputVolumes);
            inputVolumeCB[i]=cb;
        }

        layout->addWidget(inputVolumeCB[i], i, 1);
    }

    ui->wdInputVolumes->setLayout(layout);

    ui->wdInputVolumes->updateGeometry();

}


void RunVolumeScriptDialog::updateInputGridControls(){

    if( ui->wdInputGrids->layout()){
        delete ui->wdInputGrids->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputGrids);
    for( int i=0; i<ui->sbInputGrids->value(); i++){

        if( !inputGridCB[i]){
            QComboBox* cb=new QComboBox( this );
            cb->clear();
            cb->addItems(m_inputGrids);
            inputGridCB[i]=cb;
        }

        layout->addWidget(inputGridCB[i], i, 1);
    }

    ui->wdInputGrids->setLayout(layout);

    ui->wdInputGrids->updateGeometry();

}

void RunVolumeScriptDialog::updateOkButton(){

    bool ok=true;

    if( ui->leResultVolume->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( m_inputVolumes.contains(ui->leResultVolume->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leResultVolume->setPalette(palette);

    ui->pbOK->setEnabled(ok);
}



void RunVolumeScriptDialog::on_pbLoad_clicked()
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

void RunVolumeScriptDialog::on_pbSave_clicked()
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
