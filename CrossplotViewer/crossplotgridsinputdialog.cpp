#include "crossplotgridsinputdialog.h"
#include "ui_crossplotgridsinputdialog.h"

CrossplotGridsInputDialog::CrossplotGridsInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotGridsInputDialog)
{
    ui->setupUi(this);

    QStringList types;
    types<<toQString(GridType::Attribute);
    types<<toQString(GridType::Horizon);
    types<<toQString(GridType::Other);
    ui->cbXType->insertItems(0, types);
    ui->cbYType->insertItems(0, types);
    ui->cbAttributeType->insertItems(0, types);

    connect( ui->cbXType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateXCombo()) );
    connect( ui->cbYType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYCombo()) );
    connect( ui->cbAttributeType, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updateAttributeCombo()) );
}

CrossplotGridsInputDialog::~CrossplotGridsInputDialog()
{
    delete ui;
}


GridType CrossplotGridsInputDialog::xType(){

    return toGridType(ui->cbXType->currentText());
}

QString CrossplotGridsInputDialog::xName(){

    return ui->cbXName->currentText();
}

GridType CrossplotGridsInputDialog::yType(){

    return toGridType(ui->cbYType->currentText());
}

QString CrossplotGridsInputDialog::yName(){

    return ui->cbYName->currentText();
}

GridType CrossplotGridsInputDialog::attributeType(){

    return toGridType(ui->cbAttributeType->currentText());
}

QString CrossplotGridsInputDialog::attributeName(){

    return ui->cbAttributeName->currentText();
}

bool CrossplotGridsInputDialog::useAttribute(){

    return ui->cbDisplayAttribute->isChecked();
}

QString CrossplotGridsInputDialog::horizonName(){

    return ui->cbHorizonName->currentText();
}

bool CrossplotGridsInputDialog::useHorizon(){

    return ui->cbUseHorizon->isChecked();
}


void CrossplotGridsInputDialog::setHorizons(QStringList l){

    horizons=l;
    updateHorizonCombo();
    updateCombos();
}

void CrossplotGridsInputDialog::setAttributeGrids(QStringList l){

    attributeGrids=l;
    updateCombos();
}

void CrossplotGridsInputDialog::setOtherGrids(QStringList l){

    otherGrids=l;
    updateCombos();
}

void CrossplotGridsInputDialog::setComboNames(GridType type, QComboBox* cb){

    cb->clear();
    switch(type)
    {
    case GridType::Attribute: cb->insertItems(0, attributeGrids);break;
    case GridType::Horizon: cb->insertItems(0, horizons); break;
    case GridType::Other: cb->insertItems(0, otherGrids); break;
    }
}

void CrossplotGridsInputDialog::updateCombos(){
    updateXCombo();
    updateYCombo();
    updateAttributeCombo();
}

void CrossplotGridsInputDialog::updateXCombo(){

    setComboNames(toGridType(ui->cbXType->currentText()), ui->cbXName );
}

void CrossplotGridsInputDialog::updateYCombo(){

    setComboNames(toGridType(ui->cbYType->currentText()), ui->cbYName );
}

void CrossplotGridsInputDialog::updateHorizonCombo(){

    ui->cbHorizonName->clear();
    ui->cbHorizonName->insertItems(0, horizons);
}


void CrossplotGridsInputDialog::updateAttributeCombo(){

    setComboNames(toGridType(ui->cbAttributeType->currentText()), ui->cbAttributeName );
}
