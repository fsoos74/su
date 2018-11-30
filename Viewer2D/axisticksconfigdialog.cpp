#include "axisticksconfigdialog.h"
#include "ui_axisticksconfigdialog.h"
#include<QDoubleValidator>

AxisTicksConfigDialog::AxisTicksConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AxisTicksConfigDialog)
{
    ui->setupUi(this);
    auto validator=new QDoubleValidator(this);
    validator->setBottom(0.000001);
    ui->leInterval->setValidator(validator);
}

AxisTicksConfigDialog::~AxisTicksConfigDialog()
{
    delete ui;
}

double AxisTicksConfigDialog::interval(){
    return ui->leInterval->text().toDouble();
}

int AxisTicksConfigDialog::subTicks(){
    return ui->sbSub->value();
}

bool AxisTicksConfigDialog::isAutomatic(){
    return ui->cbAuto->isChecked();
}

void AxisTicksConfigDialog::setInterval(double x){
    ui->leInterval->setText(QString::number(x));
}

void AxisTicksConfigDialog::setSubTicks(int i){
    ui->sbSub->setValue(i);
}

void AxisTicksConfigDialog::setAutomatic(bool on){
    ui->cbAuto->setChecked(on);
}
