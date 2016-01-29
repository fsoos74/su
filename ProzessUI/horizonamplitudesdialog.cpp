#include "horizonamplitudesdialog.h"
#include "ui_horizonamplitudesdialog.h"

#include<QPushButton>

#include<avoproject.h>

HorizonAmplitudesDialog::HorizonAmplitudesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::HorizonAmplitudesDialog)
{
    ui->setupUi(this);
    connect( ui->leGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

HorizonAmplitudesDialog::~HorizonAmplitudesDialog()
{
    delete ui;
}

void HorizonAmplitudesDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void HorizonAmplitudesDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}

void HorizonAmplitudesDialog::setMethods( const QStringList& h){
    ui->cbMethod->clear();
    ui->cbMethod->addItems(h);
}


QMap<QString,QString> HorizonAmplitudesDialog::params(){

    QMap<QString, QString> p;

    QString fullAmplitudeGridName=createFullGridName( GridType::Other, ui->leGrid->text());

    p.insert( QString("grid"), fullAmplitudeGridName );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("method"), ui->cbMethod->currentText());

    p.insert( QString("half-samples"), QString::number(ui->sbSamples->value()/2) );

    if( ui->cbDisplay->isChecked()){
        p.insert(QString("display-grid"), fullAmplitudeGridName);
    }

    return p;
}

void HorizonAmplitudesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leGrid->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedGrids().contains(ui->leGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGrid->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


