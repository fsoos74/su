#include "gridrunuserscriptdialog.h"
#include "ui_gridrunuserscriptdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QPushButton>
#include<avoproject.h>

GridRunUserScriptDialog::GridRunUserScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridRunUserScriptDialog)
{
    ui->setupUi(this);

    // set monospace font for code editing
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPixelSize(14);
    ui->teScript->setFont(font);


    inputCB.resize(ui->sbInputGrids->maximum()); // fills with nullptr

    connect( ui->sbInputGrids, SIGNAL(valueChanged(int)),
             this, SLOT(updateInputGridControls()));

    connect( ui->leResultGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect( ui->teScript, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );


    updateInputGridControls();

    updateOkButton();
}

GridRunUserScriptDialog::~GridRunUserScriptDialog()
{
    delete ui;
}

void GridRunUserScriptDialog::setInputGrids( const QStringList& l){

    if( l==m_inputGrids) return;

    m_inputGrids=l;

    for( QComboBox* cb : inputCB){
        if( cb ){
            cb->clear();
            cb->addItems(m_inputGrids);
        }
    }

    updateOkButton();
}

QMap<QString,QString> GridRunUserScriptDialog::params(){

    QMap<QString, QString> p;

    QString fullGridName=createFullGridName( GridType::Attribute,
                                             ui->leResultGrid->text() );

    p.insert( QString("result"), fullGridName);

    p.insert( QString("n_input_grids"), QString::number(ui->sbInputGrids->value()));
    for( int i=0; i<ui->sbInputGrids->value(); i++){
        p.insert( QString("input_grid_")+QString::number(i+1),
                inputCB[i]->currentText() );
    }


    p.insert( QString("script"), ui->teScript->toPlainText() );

    return p;
}

void GridRunUserScriptDialog::updateInputGridControls(){

    if( ui->wdInputGrids->layout()){
        delete ui->wdInputGrids->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputGrids);
    for( int i=0; i<ui->sbInputGrids->value(); i++){

        if( !inputCB[i]){
            QComboBox* cb=new QComboBox( this );
            cb->clear();
            cb->addItems(m_inputGrids);
            inputCB[i]=cb;
        }

        layout->addWidget(inputCB[i], i, 1);
    }

    ui->wdInputGrids->setLayout(layout);

    ui->wdInputGrids->updateGeometry();

}

void GridRunUserScriptDialog::updateOkButton(){

    bool ok=true;

    if( ui->leResultGrid->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( m_inputGrids.contains(ui->leResultGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leResultGrid->setPalette(palette);

    ui->pbOK->setEnabled(ok);
}



void GridRunUserScriptDialog::on_pbLoad_clicked()
{
    QString fn=QFileDialog::getOpenFileName(this,
        "Open Script", QDir::homePath(), "*.py");

    if( fn.isNull()) return;

    QFile theFile(fn);
    theFile.open(QIODevice::ReadOnly);
    // add error checking code HERE

    QTextStream textStream(&theFile);
    ui->teScript->setPlainText(textStream.readAll());
    theFile.close();
}

void GridRunUserScriptDialog::on_pbSave_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this,
        "Save Script", QDir::homePath(), "*.py");


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
