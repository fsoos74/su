#include "gridcolorcompositeinputdialog.h"
#include "ui_gridcolorcompositeinputdialog.h"

GridColorCompositeInputDialog::GridColorCompositeInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridColorCompositeInputDialog)
{
    ui->setupUi(this);

    QStringList types;
    types<<toQString(GridType::Attribute);
    types<<toQString(GridType::Horizon);
    types<<toQString(GridType::Other);
    ui->cbRedType->insertItems(0, types);
    ui->cbGreenType->insertItems(0, types);
    ui->cbBlueType->insertItems(0, types);

    connect( ui->cbRedType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRedCombo()) );
    connect( ui->cbGreenType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGreenCombo()) );
    connect( ui->cbBlueType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBlueCombo()) );
}

GridColorCompositeInputDialog::~GridColorCompositeInputDialog()
{
    delete ui;
}


GridType GridColorCompositeInputDialog::redType(){

    return toGridType(ui->cbRedType->currentText());
}

QString GridColorCompositeInputDialog::redName(){

    return ui->cbRedName->currentText();
}

GridType GridColorCompositeInputDialog::greenType(){

    return toGridType(ui->cbGreenType->currentText());
}

QString GridColorCompositeInputDialog::greenName(){

    return ui->cbGreenName->currentText();
}

GridType GridColorCompositeInputDialog::blueType(){

    return toGridType(ui->cbBlueType->currentText());
}

QString GridColorCompositeInputDialog::blueName(){

    return ui->cbBlueName->currentText();
}


void GridColorCompositeInputDialog::setHorizons(QStringList l){

    horizons=l;
    updateCombos();
}

void GridColorCompositeInputDialog::setAttributeGrids(QStringList l){

    attributeGrids=l;
    updateCombos();
}

void GridColorCompositeInputDialog::setOtherGrids(QStringList l){

    otherGrids=l;
    updateCombos();
}

void GridColorCompositeInputDialog::setComboNames(GridType type, QComboBox* cb){

    cb->clear();
    switch(type)
    {
    case GridType::Attribute: cb->insertItems(0, attributeGrids);break;
    case GridType::Horizon: cb->insertItems(0, horizons); break;
    case GridType::Other: cb->insertItems(0, otherGrids); break;
    }
}

void GridColorCompositeInputDialog::updateCombos(){
    updateRedCombo();
    updateGreenCombo();
    updateBlueCombo();
}

void GridColorCompositeInputDialog::updateRedCombo(){

    setComboNames(toGridType(ui->cbRedType->currentText()), ui->cbRedName );
}

void GridColorCompositeInputDialog::updateGreenCombo(){

    setComboNames(toGridType(ui->cbGreenType->currentText()), ui->cbGreenName );
}

void GridColorCompositeInputDialog::updateBlueCombo(){

    setComboNames(toGridType(ui->cbBlueType->currentText()), ui->cbBlueName );
}
