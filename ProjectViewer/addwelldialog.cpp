#include "addwelldialog.h"
#include "ui_addwelldialog.h"

#include<QDoubleValidator>

AddWellDialog::AddWellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddWellDialog)
{
    ui->setupUi(this);
    auto validator=new QDoubleValidator(this);
    ui->leX->setValidator(validator);
    ui->leY->setValidator(validator);
    ui->leZ->setValidator(validator);
}

AddWellDialog::~AddWellDialog()
{
    delete ui;
}

WellInfo AddWellDialog::wellInfo() const{
    WellInfo info;
    info.setName(ui->leName->text());
    info.setUWI(ui->leUwi->text());
    info.setX(ui->leX->text().toDouble());
    info.setY(ui->leY->text().toDouble());
    info.setZ(ui->leZ->text().toDouble());
    return info;
}

void AddWellDialog::setWellInfo(const WellInfo & info){
    ui->leName->setText(info.name());
    ui->leUwi->setText(info.uwi());
    ui->leX->setText(QString::number(info.x()));
    ui->leY->setText(QString::number(info.y()));
    ui->leZ->setText(QString::number(info.z()));
}
