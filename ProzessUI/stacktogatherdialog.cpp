#include "stacktogatherdialog.h"
#include "ui_stacktogatherdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QPushButton>
#include<avoproject.h>

StackToGatherDialog::StackToGatherDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::StackToGatherDialog)
{
    ui->setupUi(this);

    inputCB.resize(ui->sbInputStacks->maximum()); // fills with nullptr

    connect( ui->sbInputStacks, SIGNAL(valueChanged(int)),
             this, SLOT(updateInputStackControls()));

    connect( ui->leOutputGather, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateInputStackControls();

    updateOkButton();
}

StackToGatherDialog::~StackToGatherDialog()
{
    delete ui;
}


void StackToGatherDialog::setInputStacks(QStringList lst){

    m_inputStacks=lst;

    for( QComboBox* cb : inputCB){
        if( cb ){
            cb->clear();
            cb->addItems(m_inputStacks);
        }
    }

    updateOkButton();
}

QMap<QString,QString> StackToGatherDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("gather"), ui->leOutputGather->text() );

    QStringList stacks;
    for( int i=0; i<ui->sbInputStacks->value(); i++){
        stacks<<inputCB[i]->currentText();
    }
    p.insert( QString("stacks"), packParamList(stacks) );

    return p;
}

void StackToGatherDialog::updateInputStackControls(){

    if( ui->wdInputStacks->layout()){
        delete ui->wdInputStacks->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputStacks);
    for( int i=0; i<ui->sbInputStacks->value(); i++){

        if( !inputCB[i]){
            QComboBox* cb=new QComboBox( this );
            cb->clear();
            cb->addItems(m_inputStacks);
            inputCB[i]=cb;
        }

        layout->addWidget(inputCB[i], i, 1);
    }

    ui->wdInputStacks->setLayout(layout);

    ui->wdInputStacks->updateGeometry();

}

void StackToGatherDialog::updateOkButton(){

    bool ok=true;

    auto oname=ui->leOutputGather->text();
    QPalette palette;
    if( oname.isEmpty()){
        ok=false;
    }
    else if( reservedDatasets().contains(oname)){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputGather->setPalette(palette);

    ui->pbOK->setEnabled(ok);
}
