#include "horizonfrequenciesdialog.h"
#include "ui_horizonfrequenciesdialog.h"

#include<QPushButton>
#include<QDoubleValidator>
#include<avoproject.h>
#include<horizonwindowposition.h>

HorizonFrequenciesDialog::HorizonFrequenciesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::HorizonFrequenciesDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinimumFrequency->setValidator(validator);
    ui->leMaximumFrequency->setValidator(validator);

    HorizonWindowPosition pos[]={HorizonWindowPosition::Above, HorizonWindowPosition::Center,
                                HorizonWindowPosition::Below};
    for( auto p : pos){
        ui->cbPosition->addItem(toQString(p));
    }

    connect( ui->leGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinimumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaximumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

HorizonFrequenciesDialog::~HorizonFrequenciesDialog()
{
    delete ui;
}

void HorizonFrequenciesDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void HorizonFrequenciesDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> HorizonFrequenciesDialog::params(){

    QMap<QString, QString> p;

    QString fullAmplitudeGridName=createFullGridName( GridType::Other, ui->leGrid->text());

    p.insert( QString("grid"), fullAmplitudeGridName );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("minimum-frequency"), ui->leMinimumFrequency->text() );

    p.insert( QString("maximum-frequency"), ui->leMaximumFrequency->text() );

    p.insert( QString("window-samples"), ui->cbWindowSamples->currentText() );

    p.insert( QString("window-position"), ui->cbPosition->currentText());

    if( ui->cbDisplay->isChecked()){
        p.insert(QString("display-grid"), fullAmplitudeGridName);
    }

    return p;
}

void HorizonFrequenciesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leGrid->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedGrids().contains(ui->leGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGrid->setPalette(palette);
    }

    {
    QPalette palette;
    if( ui->leMaximumFrequency->text().toDouble() < ui->leMinimumFrequency->text().toDouble()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinimumFrequency->setPalette(palette);
    ui->leMaximumFrequency->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


