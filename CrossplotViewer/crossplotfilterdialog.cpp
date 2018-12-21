#include "crossplotfilterdialog.h"
#include "ui_crossplotfilterdialog.h"

#include<QIntValidator>
#include<QDoubleValidator>

CrossplotFilterDialog::CrossplotFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotFilterDialog)
{
    ui->setupUi(this);

    auto ivalid=new QIntValidator(this);
    ui->leMinIL->setValidator(ivalid);
    ui->leMaxIL->setValidator(ivalid);
    ui->leMinXL->setValidator(ivalid);
    ui->leMaxXL->setValidator(ivalid);
    ui->leMinDS->setValidator(ivalid);
    ui->leMaxDS->setValidator(ivalid);
    auto dvalid=new QDoubleValidator(this);
    ui->leMinA->setValidator(dvalid);
    ui->leMaxA->setValidator(dvalid);
}

CrossplotFilterDialog::~CrossplotFilterDialog()
{
    delete ui;
}


int CrossplotFilterDialog::minInline(){
    return ui->leMinIL->text().toInt();
}

int CrossplotFilterDialog::maxInline(){
    return ui->leMaxIL->text().toInt();
}

int CrossplotFilterDialog::minCrossline(){
    return ui->leMinXL->text().toInt();
}

int CrossplotFilterDialog::maxCrossline(){
    return ui->leMaxXL->text().toInt();
}

int CrossplotFilterDialog::minZ(){
    return ui->leMinZ->text().toInt();
}

int CrossplotFilterDialog::maxZ(){
    return ui->leMaxZ->text().toInt();
}


double CrossplotFilterDialog::minAttribute(){
    return ui->leMinA->text().toDouble();
}

double CrossplotFilterDialog::maxAttribute(){
    return ui->leMaxA->text().toDouble();
}

int CrossplotFilterDialog::minDataset(){
    return ui->leMinDS->text().toInt();
}

int CrossplotFilterDialog::maxDataset(){
    return ui->leMaxDS->text().toInt();
}


void CrossplotFilterDialog::setMinInline(int i){
    ui->leMinIL->setText(QString::number(i));
}

void CrossplotFilterDialog::setMaxInline(int i){
    ui->leMaxIL->setText(QString::number(i));
}

void CrossplotFilterDialog::setMinCrossline(int i){
    ui->leMinXL->setText(QString::number(i));
}

void CrossplotFilterDialog::setMaxCrossline(int i){
    ui->leMaxXL->setText(QString::number(i));
}

void CrossplotFilterDialog::setMinZ(int i){
    ui->leMinZ->setText(QString::number(i));
}

void CrossplotFilterDialog::setMaxZ(int i){
    ui->leMaxZ->setText(QString::number(i));
}


void CrossplotFilterDialog::setMinAttribute(double d){
    ui->leMinA->setText(QString::number(d));
}

void CrossplotFilterDialog::setMaxAttribute(double d){
    ui->leMaxA->setText(QString::number(d));
}

void CrossplotFilterDialog::setMinDataset(int i){
    ui->leMinDS->setText(QString::number(i));
}

void CrossplotFilterDialog::setMaxDataset(int i){
    ui->leMaxDS->setText(QString::number(i));
}
