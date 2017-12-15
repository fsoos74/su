#include "amplitudecurvedataselectiondialog.h"
#include "ui_amplitudecurvedataselectiondialog.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <limits>

AmplitudeCurveDataSelectionDialog::AmplitudeCurveDataSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AmplitudeCurveDataSelectionDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    ui->leInline->setValidator(validator);
    ui->leCrossline->setValidator(validator);

    QDoubleValidator* doubleValidator=new QDoubleValidator(this);
    //doubleValidator->setBottom(0);
    ui->leMinOffset->setValidator(doubleValidator);
    ui->leMaxOffset->setValidator(doubleValidator);
    ui->leMSec->setValidator(doubleValidator);
    ui->leDepth->setValidator(doubleValidator);

    QDoubleValidator* azimuthValidator=new QDoubleValidator(this);
    azimuthValidator->setRange(0,180);
    ui->leMinAzimuth->setValidator(azimuthValidator);
    ui->leMaxAzimuth->setValidator(azimuthValidator);
}

AmplitudeCurveDataSelectionDialog::~AmplitudeCurveDataSelectionDialog()
{
    delete ui;
}


QString AmplitudeCurveDataSelectionDialog::datasetName(){
    return ui->cbDataset->currentText();
}

int AmplitudeCurveDataSelectionDialog::inlineNumber(){
    return ui->leInline->text().toInt();
}

int AmplitudeCurveDataSelectionDialog::crosslineNumber(){
    return ui->leCrossline->text().toInt();
}

double AmplitudeCurveDataSelectionDialog::time(){
    return 0.001*ui->leMSec->text().toDouble();
}

double AmplitudeCurveDataSelectionDialog::minimumOffset(){

    if( ui->cbRestrictOffset->isChecked()){
        return ui->leMinOffset->text().toDouble();
    }
    else{
        return std::numeric_limits<double>::lowest();
    }
}

double AmplitudeCurveDataSelectionDialog::maximumOffset(){

    if( ui->cbRestrictOffset->isChecked()){
        return ui->leMaxOffset->text().toDouble();
    }
    else{
        return std::numeric_limits<double>::max();
    }
}

double AmplitudeCurveDataSelectionDialog::maximumAzimuth(){

    if( ui->cbRestrictAzimuth->isChecked()){
        return ui->leMaxAzimuth->text().toDouble();
    }
    else{
        return 360.;
    }
}

double AmplitudeCurveDataSelectionDialog::minimumAzimuth(){

    if( ui->cbRestrictAzimuth->isChecked()){
        return ui->leMinAzimuth->text().toDouble();
    }
    else{
        return 0.;
    }
}

double AmplitudeCurveDataSelectionDialog::depth(){

    return ui->leDepth->text().toDouble();
}

int AmplitudeCurveDataSelectionDialog::inlineSize(){

    if( ui->cbSuperGather->isChecked()){
        return ui->sbInlines->value();
    }
    else{
        return 1;
    }
}

int AmplitudeCurveDataSelectionDialog::crosslineSize(){

    if( ui->cbSuperGather->isChecked()){
        return ui->sbCrosslines->value();
    }
    else{
        return 1;
    }
}

void AmplitudeCurveDataSelectionDialog::setDatasetNames(const QStringList & l){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(l);
}

void AmplitudeCurveDataSelectionDialog::setInlineNumber(int il){

    ui->leInline->setText(QString::number(il));
}

void AmplitudeCurveDataSelectionDialog::setCrosslineNumber(int xl){

    ui->leCrossline->setText(QString::number(xl));
}

void AmplitudeCurveDataSelectionDialog::setTime(double secs){
    ui->leMSec->setText(QString::number(1000*secs));
}

void AmplitudeCurveDataSelectionDialog::setMinimumOffset(const double & minOffset){

    ui->leMinOffset->setText(QString::number(minOffset));
}

void AmplitudeCurveDataSelectionDialog::setMaximumOffset(const double & maxOffset){

    ui->leMaxOffset->setText(QString::number(maxOffset));
}

void AmplitudeCurveDataSelectionDialog::setMaximumAzimuth(const double az){

    ui->leMaxAzimuth->setText(QString::number(az));
}

void AmplitudeCurveDataSelectionDialog::setMinimumAzimuth(const double az){

    ui->leMinAzimuth->setText(QString::number(az));
}

void AmplitudeCurveDataSelectionDialog::setDepth(double d){

    ui->leDepth->setText( QString::number(d));
}

void AmplitudeCurveDataSelectionDialog::on_pbAdd_clicked()
{

    AmplitudeCurveDefinition def;
    def.dataset=datasetName();
    def.inlineNumber=inlineNumber();
    def.time=time();
    def.crosslineNumber=crosslineNumber();
    def.inlineSize=inlineSize();
    def.crosslineSize=crosslineSize();
    def.minimumOffset=minimumOffset();
    def.maximumOffset=maximumOffset();
    def.minimumAzimuth=minimumAzimuth();
    def.maximumAzimuth=maximumAzimuth();
    def.depth=depth();

    emit curveDataSelected(def );
}
