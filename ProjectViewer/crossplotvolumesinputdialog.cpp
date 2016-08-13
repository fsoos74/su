#include "crossplotvolumesinputdialog.h"
#include "ui_crossplotvolumesinputdialog.h"

CrossplotVolumesInputDialog::CrossplotVolumesInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotVolumesInputDialog)
{
    ui->setupUi(this);
}

CrossplotVolumesInputDialog::~CrossplotVolumesInputDialog()
{
    delete ui;
}


QString CrossplotVolumesInputDialog::xName(){
    return ui->cbVolumeX->currentText();
}

QString CrossplotVolumesInputDialog::yName(){
    return ui->cbVolumeY->currentText();
}

bool CrossplotVolumesInputDialog::useAttribute(){
    return ui->cbAttribute->isChecked();
}

QString CrossplotVolumesInputDialog::attributeName(){
    return ui->cbVolumeAttribute->currentText();
}

void CrossplotVolumesInputDialog::setVolumes(QStringList l){
    ui->cbVolumeX->clear();
    ui->cbVolumeX->insertItems(0, l);

    ui->cbVolumeY->clear();
    ui->cbVolumeY->insertItems(0, l);

    ui->cbVolumeAttribute->clear();
    ui->cbVolumeAttribute->insertItems(0, l);
}

void CrossplotVolumesInputDialog::setUseGrids(bool on){
    ui->cbAttribute->setChecked(on);
}
