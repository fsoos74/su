#include "horizonsemblancedialog.h"
#include "ui_horizonsemblancedialog.h"

#include<QPushButton>
#include<avoproject.h>

HorizonSemblanceDialog::HorizonSemblanceDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::HorizonSemblanceDialog)
{
    ui->setupUi(this);

    connect( ui->leGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

HorizonSemblanceDialog::~HorizonSemblanceDialog()
{
    delete ui;
}

void HorizonSemblanceDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void HorizonSemblanceDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> HorizonSemblanceDialog::params(){

    QMap<QString, QString> p;

    QString fullGridName=createFullGridName( GridType::Other, ui->leGrid->text() );

    p.insert( QString("grid"), fullGridName);

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("half-ilines"), QString::number(ui->sbInlines->value()/2) );
    p.insert( QString("half-xlines"), QString::number(ui->sbCrosslines->value()/2) );
    p.insert( QString("half-samples"), QString::number(ui->sbSamples->value()/2) );

    if( ui->cbDisplay->isChecked()){
        p.insert( QString("display-grid"), fullGridName);
    }

    return p;
}

void HorizonSemblanceDialog::updateOkButton(){

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
