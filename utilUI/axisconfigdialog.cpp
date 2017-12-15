#include "axisconfigdialog.h"
#include "ui_axisconfigdialog.h"

#include<QDoubleValidator>
#include<QPushButton>


bool AxisConfigDialog::configAxis(Axis* axis, const QString& title){

    AxisConfigDialog dlg;
    dlg.setWindowTitle(title);

    dlg.setName(axis->name());
    dlg.setUnit(axis->unit());
    int t = (axis->type()==Axis::Type::Linear) ? 0 : 1;
    dlg.setType(t);
    dlg.setMinimum(axis->min());
    dlg.setMaximum(axis->max());
    dlg.setReversed(axis->isReversed());
    dlg.setAutomatic(axis->isAutomatic());
    dlg.setInterval(axis->interval());
    dlg.setSubTicks(axis->subTickCount());
    dlg.setLabelSubTicks(axis->labelSubTicks());
    dlg.setLabelPrecision(axis->labelPrecision());

    if(dlg.exec()!=QDialog::Accepted) return false;

    axis->setName(dlg.name());
    axis->setUnit(dlg.unit());
    axis->setAutomaticInterval(dlg.isAutomatic());  // do this first because intervals will be computed upon set range if automatic
    // only change view range if type has changed
    // this avoids negative values and other complications for log scales
    if( dlg.type()!=t || dlg.minimum()!=axis->min() || dlg.maximum()!=axis->max()){
        axis->setViewRange(dlg.minimum(), dlg.maximum());   // do this before set range
    }
    axis->setRange(dlg.minimum(), dlg.maximum());
    Axis::Type tt=(dlg.type()==0) ? Axis::Type::Linear : Axis::Type::Logarithmic;
    axis->setType(tt);   // avoid zero or negative min value when setting type, thus do this after set range

    axis->setReversed(dlg.isReversed());
    if(!dlg.isAutomatic()){
        axis->setInterval(dlg.interval());
    }
    axis->setSubTickCount(dlg.subTicks());
    axis->setLabelSubTicks(dlg.labelSubTicks());
    axis->setLabelPrecision(dlg.labelPrecision());
    return true;
}

AxisConfigDialog::AxisConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AxisConfigDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinimum->setValidator(validator);
    ui->leMaximum->setValidator(validator);

    QDoubleValidator* posvalidator=new QDoubleValidator(this);
    ui->leInterval->setValidator(posvalidator);
    posvalidator->setBottom(0);

    connect(ui->leMinimum, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));
    connect(ui->leMaximum, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));
}

AxisConfigDialog::~AxisConfigDialog()
{
    delete ui;
}

QString AxisConfigDialog::name(){
    return ui->leName->text();
}

QString AxisConfigDialog::unit(){
    return ui->leUnit->text();
}

qreal AxisConfigDialog::minimum(){
    return ui->leMinimum->text().toDouble();
}

qreal AxisConfigDialog::maximum(){
    return ui->leMaximum->text().toDouble();
}

int AxisConfigDialog::type(){
    return ui->cbType->currentIndex();
}

bool AxisConfigDialog::isReversed(){
    return ui->cbReverse->isChecked();
}

bool AxisConfigDialog::isAutomatic(){
    return ui->cbAutomatic->isChecked();
}

qreal AxisConfigDialog::interval(){
    return ui->leInterval->text().toDouble();
}

int AxisConfigDialog::subTicks(){
    return ui->sbSubTicks->value();
}

bool AxisConfigDialog::labelSubTicks(){
    return ui->cbLabelSubTicks->isChecked();
}

int AxisConfigDialog::labelPrecision(){
    return ui->sbLabelPrecision->value();
}

void AxisConfigDialog::setName(QString name){
    ui->leName->setText(name);
}

void AxisConfigDialog::setUnit(QString unit){
    ui->leUnit->setText(unit);
}

void AxisConfigDialog::setMinimum(qreal x){
    ui->leMinimum->setText(QString::number(x));
}

void AxisConfigDialog::setMaximum(qreal x){
    ui->leMaximum->setText(QString::number(x));
}

void AxisConfigDialog::setType(int i){
    ui->cbType->setCurrentIndex(i);
}

void AxisConfigDialog::setReversed(bool on){
    ui->cbReverse->setChecked(on);
}

void AxisConfigDialog::setAutomatic(bool on){
    ui->cbAutomatic->setChecked(on);
}

void AxisConfigDialog::setInterval(qreal x){
    ui->leInterval->setText(QString::number(x));
}

void AxisConfigDialog::setSubTicks(int i){
    ui->sbSubTicks->setValue(i);
}

void AxisConfigDialog::setLabelSubTicks(bool on){
    ui->cbLabelSubTicks->setChecked(on);
}

void AxisConfigDialog::setLabelPrecision(int p){
    ui->sbLabelPrecision->setValue(p);
}

void AxisConfigDialog::updateOkButton(){

    bool ok=true;

    // test scale range
    {
    QPalette palette;
    if( ui->leMinimum->text().toDouble() > ui->leMaximum->text().toDouble() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    if( type()==1 && ( ui->leMinimum->text().toDouble()<=0 || ui->leMaximum->text().toDouble()<=0) ){ // logarithmic allows no values <=0
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinimum->setPalette(palette);
    ui->leMaximum->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void AxisConfigDialog::on_cbType_currentIndexChanged(int index)
{
    if(index!=0){
        ui->leInterval->setText("10");
        ui->leInterval->setDisabled(true);
    }
    updateOkButton();
}
