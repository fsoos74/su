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
    doubleValidator->setBottom(0);
    ui->leMaxOffset->setValidator(doubleValidator);
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

QString AmplitudeCurveDataSelectionDialog::horizonName(){
    return ui->cbHorizon->currentText();
}

int AmplitudeCurveDataSelectionDialog::inlineNumber(){
    return ui->leInline->text().toInt();
}

int AmplitudeCurveDataSelectionDialog::crosslineNumber(){
    return ui->leCrossline->text().toInt();
}

QString AmplitudeCurveDataSelectionDialog::reductionMethod(){
    return ui->cbMethod->currentText();
}

int AmplitudeCurveDataSelectionDialog::numberOfSamples(){
    return ui->sbSamples->value();
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



void AmplitudeCurveDataSelectionDialog::setInlineNumber(int il){

    ui->leInline->setText(QString::number(il));
}

void AmplitudeCurveDataSelectionDialog::setCrosslineNumber(int xl){

    ui->leCrossline->setText(QString::number(xl));
}

void AmplitudeCurveDataSelectionDialog::setDatasetNames( const QStringList& names){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(names);
}

void AmplitudeCurveDataSelectionDialog::setHorizonNames( const QStringList& names){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(names);
}

void AmplitudeCurveDataSelectionDialog::setReductionMethods( const QStringList& h){
    ui->cbMethod->clear();
    ui->cbMethod->addItems(h);
}

// will only select existing method
void AmplitudeCurveDataSelectionDialog::setCurrentReductionMethod(QString s){
    auto idx=ui->cbMethod->findText(s);
    if( idx >= 0) ui->cbMethod->setCurrentIndex(idx);
}

void AmplitudeCurveDataSelectionDialog::setNumberOfSamples( int n ){
    ui->sbSamples->setValue(n);
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
    def.horizon=horizonName();
    def.inlineNumber=inlineNumber();
    def.crosslineNumber=crosslineNumber();
    def.inlineSize=inlineSize();
    def.crosslineSize=crosslineSize();
    def.maximumOffset=maximumOffset();
    def.minimumAzimuth=minimumAzimuth();
    def.maximumAzimuth=maximumAzimuth();
    def.depth=depth();
    def.reductionMethod=reductionMethod();
    def.windowSize=numberOfSamples();


    emit curveDataSelected(def );
}
