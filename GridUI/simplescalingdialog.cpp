#include "simplescalingdialog.h"
#include "ui_simplescalingdialog.h"
#include<QDoubleValidator>
#include<iostream>

namespace sliceviewer {

SimpleScalingDialog::SimpleScalingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimpleScalingDialog)
{
    std::cout<<"SSDLG"<<std::endl<<std::flush;
    ui->setupUi(this);
    auto validator=new QDoubleValidator(this);
    ui->leMinimum->setValidator(validator);
    ui->leMaximum->setValidator(validator);
    ui->lePower->setValidator(validator);
}

SimpleScalingDialog::~SimpleScalingDialog()
{
    delete ui;
}

double SimpleScalingDialog::minimum(){
    return ui->leMinimum->text().toDouble();
}

double SimpleScalingDialog::maximum(){
    return ui->leMaximum->text().toDouble();
}

double SimpleScalingDialog::power(){
    return ui->lePower->text().toDouble();
}

void SimpleScalingDialog::setMinimum(double minimum){
    ui->leMinimum->setText(QString::number(minimum));
}

void SimpleScalingDialog::setMaximum(double maximum){
    ui->leMaximum->setText(QString::number(maximum));
}

void SimpleScalingDialog::setPower(double power){
    ui->lePower->setText(QString::number(power));
}

}
