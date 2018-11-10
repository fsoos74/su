#include "volumepropertydialog.h"
#include "ui_volumepropertydialog.h"

#include<QDoubleValidator>
#include<domain.h>
#include<volumetype.h>


VolumePropertyDialog::VolumePropertyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumePropertyDialog)
{
    ui->setupUi(this);

    auto validator=new QDoubleValidator(this);
    ui->leZ0->setValidator(validator);
    ui->leDZ->setValidator(validator);

    ui->cbDomain->addItem(toQString(Domain::Time));
    ui->cbDomain->addItem(toQString(Domain::Depth));
    ui->cbDomain->addItem(toQString(Domain::Other));

    ui->cbType->addItem(toQString(VolumeType::AVO));
    ui->cbType->addItem(toQString(VolumeType::IVEL));
    ui->cbType->addItem(toQString(VolumeType::AVEL));
    ui->cbType->addItem(toQString(VolumeType::Other));
}

VolumePropertyDialog::~VolumePropertyDialog()
{
    delete ui;
}


void VolumePropertyDialog::setDomain(Domain d){
    ui->cbDomain->setCurrentText(toQString(d));
}

void VolumePropertyDialog::setType(VolumeType t){
    ui->cbType->setCurrentText(toQString(t));
}

void VolumePropertyDialog::setZ0(qreal z){
    ui->leZ0->setText(QString::number(z));
}

void VolumePropertyDialog::setDZ(qreal dz){
    ui->leDZ->setText(QString::number(dz));
}

Domain VolumePropertyDialog::domain(){
    return toDomain(ui->cbDomain->currentText());
}


VolumeType VolumePropertyDialog::type(){
    return toVolumeType(ui->cbType->currentText());
}


qreal VolumePropertyDialog::z0(){
    return ui->leZ0->text().toDouble();
}


qreal VolumePropertyDialog::dz(){
    return ui->leDZ->text().toDouble();
}
