#include "editwelldialog.h"
#include "ui_editwelldialog.h"

#include<QDoubleValidator>

EditWellDialog::EditWellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditWellDialog)
{
    ui->setupUi(this);

    auto validator = new QDoubleValidator(this);
    ui->leX->setValidator(validator);
    ui->leY->setValidator(validator);
    ui->leZ->setValidator(validator);
}

EditWellDialog::~EditWellDialog()
{
    delete ui;
}


QString EditWellDialog::uwi(){
    return ui->leUWI->text();
}

QString EditWellDialog::name(){
    return ui->leName->text();
}

qreal EditWellDialog::x(){
    return ui->leX->text().toDouble();
}

qreal EditWellDialog::y(){
    return ui->leY->text().toDouble();
}

qreal EditWellDialog::z(){
    return ui->leZ->text().toDouble();
}

void EditWellDialog::setUWI(QString s){
    ui->leUWI->setText(s);
}

void EditWellDialog::setName(QString s){
    ui->leName->setText(s);
}

void EditWellDialog::setX(qreal r){
    ui->leX->setText(QString::number(r));
}

void EditWellDialog::setY(qreal r){
    ui->leY->setText(QString::number(r));
}

void EditWellDialog::setZ(qreal r){
    ui->leZ->setText(QString::number(r));
}
