#include "cropdatasetdialog.h"
#include "ui_cropdatasetdialog.h"

#include <QPushButton>
#include <QDoubleValidator>

CropDatasetDialog::CropDatasetDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CropDatasetDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinOffset->setValidator(validator);
    ui->leMaxOffset->setValidator(validator);

    connect( ui->cbInputDataset, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxIline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMinXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->sbMaxXline, SIGNAL(valueChanged(int)), this, SLOT(updateOkButton()) );
    connect( ui->leMinOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

CropDatasetDialog::~CropDatasetDialog()
{
    delete ui;
}


void CropDatasetDialog::setInputDatasets(const QStringList & l){

    ui->cbInputDataset->clear();
    ui->cbInputDataset->addItems(l);
    updateOkButton();
}

void CropDatasetDialog::setInlineRange(int min, int max){

    ui->sbMinIline->setRange(min,max);
    ui->sbMaxIline->setRange(min,max);
    ui->sbMinIline->setValue(min);
    ui->sbMaxIline->setValue(max);
}

void CropDatasetDialog::setCrosslineRange(int min, int max){

    ui->sbMinXline->setRange(min,max);
    ui->sbMaxXline->setRange(min,max);
    ui->sbMinXline->setValue(min);
    ui->sbMaxXline->setValue(max);
}

void CropDatasetDialog::setMSecRange(int min, int max){

    ui->sbMinMSec->setRange(min,max);
    ui->sbMaxMSec->setRange(min,max);
    ui->sbMinMSec->setValue(min);
    ui->sbMaxMSec->setValue(max);
}

void CropDatasetDialog::setOffsetRange(float min, float max){

    ui->leMinOffset->setText(QString::number(min));
    ui->leMaxOffset->setText(QString::number(max));
}


QMap<QString,QString> CropDatasetDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-dataset"), ui->cbInputDataset->currentText() );

    p.insert( QString("min-iline"), QString::number(ui->sbMinIline->value()));
    p.insert( QString("max-iline"), QString::number(ui->sbMaxIline->value()));

    p.insert( QString("min-xline"), QString::number(ui->sbMinXline->value()));
    p.insert( QString("max-xline"), QString::number(ui->sbMaxXline->value()));

    p.insert( QString("min-msec"), QString::number( ui->sbMinMSec->value()));
    p.insert( QString("max-msec"), QString::number( ui->sbMaxMSec->value()));

    float minOffset=(ui->cbRestrictOffsets->isChecked()) ? ui->leMinOffset->text().toFloat() : std::numeric_limits<float>::lowest();
    float maxOffset=(ui->cbRestrictOffsets->isChecked()) ? ui->leMaxOffset->text().toFloat() : std::numeric_limits<float>::max();
    p.insert( QString("min-offset"), QString::number(minOffset));
    p.insert( QString("max-offset"), QString::number(maxOffset));

    return p;
}

void CropDatasetDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputDataset->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedDatasets().contains(ui->leOutputDataset->text())) {  // output name exists
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

    // test offset range
    {
    QPalette palette;
    if( ui->cbRestrictOffsets->isChecked() &&
            ui->leMinOffset->text().toFloat() > ui->leMaxOffset->text().toFloat() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinOffset->setPalette(palette);
    ui->leMaxOffset->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

