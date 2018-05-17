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
    inputLE.resize(ui->sbInputStacks->maximum()); // fills with nullptr
    leValidator=new QDoubleValidator(this);

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


void StackToGatherDialog::setProject(AVOProject * prj){
    if( prj==m_project) return;
    m_project=prj;
    updateOkButton();
}

QMap<QString,QString> StackToGatherDialog::params(){

    QMap<QString, QString> p;

    p.insert( "gather", ui->leOutputGather->text() );

    QStringList stacks;
    for( int i=0; i<ui->sbInputStacks->value(); i++){
        stacks<<inputCB[i]->currentText();
    }
    p.insert( QString("stacks"), packParamList(stacks) );

    QStringList values;
    for( int i=0; i<ui->sbInputStacks->value(); i++){
        values<<inputLE[i]->text();
    }
    p.insert( QString("values"), packParamList(values) );

    return p;
}

void StackToGatherDialog::updateInputStackControls(){

    if( ui->wdInputStacks->layout()){
        delete ui->wdInputStacks->layout();
    }

    QGridLayout* layout=new QGridLayout(ui->wdInputStacks);
    //layout->setColumnStretch(0,1);
    //layout->setColumnStretch(1,0);
    //layout->setColumnMinimumWidth(1,50);
    for( int i=0; i<ui->sbInputStacks->value(); i++){

        if( !inputCB[i]){
            QComboBox* cb=new QComboBox( this );
            cb->clear();
            cb->addItems(m_inputStacks);
            inputCB[i]=cb;
            QLineEdit* le=new QLineEdit(this);
            le->setValidator(leValidator);
            le->setText(QString::number(i+1));
            le->setFixedWidth(100);
            inputLE[i]=le;
        }

        layout->addWidget(inputCB[i], i, 1);
        layout->addWidget(inputLE[i], i, 2);
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
