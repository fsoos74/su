#include "extractdatasetdialog.h"
#include "ui_extractdatasetdialog.h"

#include <QPushButton>


ExtractDatasetDialog::ExtractDatasetDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExtractDatasetDialog)
{
    ui->setupUi(this);

    connect( ui->cbInputVolume, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinMsec, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxMsec, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
}

ExtractDatasetDialog::~ExtractDatasetDialog()
{
    delete ui;
}


void ExtractDatasetDialog::setInputVolumes(const QStringList & l){

    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(l);
    updateOkButton();
}

void ExtractDatasetDialog::setInlineRange(int min, int max){

    ui->sbMinIline->setRange(min,max);
    ui->sbMaxIline->setRange(min,max);
    ui->sbMinIline->setValue(min);
    ui->sbMaxIline->setValue(max);
}

void ExtractDatasetDialog::setCrosslineRange(int min, int max){

    ui->sbMinXline->setRange(min,max);
    ui->sbMaxXline->setRange(min,max);
    ui->sbMinXline->setValue(min);
    ui->sbMaxXline->setValue(max);
}


void ExtractDatasetDialog::setTimeRange(int min, int max){

    ui->sbMinMsec->setRange(min,max);
    ui->sbMaxMsec->setRange(min,max);
    ui->sbMinMsec->setValue(min);
    ui->sbMaxMsec->setValue(max);
}


QMap<QString,QString> ExtractDatasetDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText() );

    p.insert( QString("min-iline"), QString::number(ui->sbMinIline->value()));
    p.insert( QString("max-iline"), QString::number(ui->sbMaxIline->value()));

    p.insert( QString("min-xline"), QString::number(ui->sbMinXline->value()));
    p.insert( QString("max-xline"), QString::number(ui->sbMaxXline->value()));

    p.insert( QString("min-msec"), QString::number(ui->sbMinMsec->value()));
    p.insert( QString("max-msec"), QString::number(ui->sbMaxMsec->value()));

    return p;
}

void ExtractDatasetDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputDataset->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedDatasets().contains( ui->leOutputDataset->text() ) ) {
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputDataset->setPalette(palette);
    }

    // text inline range
    {
    QPalette palette;
    if( ui->sbMinIline->value() > ui->sbMaxIline->value() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->sbMinIline->setPalette(palette);
    ui->sbMaxIline->setPalette(palette);
    }

    // text crossline range
    {
    QPalette palette;
    if( ui->sbMinXline->value() > ui->sbMaxXline->value() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->sbMinXline->setPalette(palette);
    ui->sbMaxXline->setPalette(palette);
    }

    // text time range
    {
    QPalette palette;
    if( ui->sbMinMsec->value() > ui->sbMaxMsec->value() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->sbMinMsec->setPalette(palette);
    ui->sbMaxMsec->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

