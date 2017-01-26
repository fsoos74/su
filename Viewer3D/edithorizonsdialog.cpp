#include "edithorizonsdialog.h"
#include "ui_edithorizonsdialog.h"
#include <QColorDialog>
#include<iostream>

EditHorizonsDialog::EditHorizonsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditHorizonsDialog)
{
    ui->setupUi(this);

    connect( ui->pbAdd, SIGNAL(clicked(bool)), this, SIGNAL(addHorizonRequested()) );

    connect( ui->cbColor, SIGNAL(colorChanged(QColor)), this, SLOT(updateParams()) );
    connect( ui->cbUseColor, SIGNAL(toggled(bool)), this, SLOT(updateParams()) );
}

EditHorizonsDialog::~EditHorizonsDialog()
{
    delete ui;
}

void EditHorizonsDialog::setHorizonManager(HorizonManager * mgr){

    if( horizonManager){

    }

    horizonManager = mgr;

    if( horizonManager){

        connect( horizonManager, SIGNAL(horizonsChanged()), this, SLOT(refreshControls() )  );
        connect( horizonManager, SIGNAL(paramsChanged(QString)), this, SLOT(horizonChanged(QString) ) );
    }

    refreshControls();

}


void EditHorizonsDialog::on_pbRemove_clicked()
{

    QString name = ui->cbHorizon->currentText();
    horizonManager->remove(name);
}


void EditHorizonsDialog::refreshControls(){

    QString name=ui->cbHorizon->currentText();

    ui->cbHorizon->clear();

    if( !horizonManager) return;

    QStringList names;
    foreach( QString name, horizonManager->names()){
        names.append(name);
    }

    ui->cbHorizon->addItems(names);
    ui->cbHorizon->setCurrentText(name);
}


void EditHorizonsDialog::horizonChanged(QString name){

    if( name != ui->cbHorizon->currentText() ) return;

    horizonParamsFromControls(name);
}

void EditHorizonsDialog::updateParams(){

    if( !horizonManager ) return;

    QString name=ui->cbHorizon->currentText();
    horizonParamsFromControls(name);
}

void EditHorizonsDialog::horizonParamsToControls( QString name ){

    if( !horizonManager ) return;

    if( !horizonManager->contains(name)) return;

    HorizonDef def=horizonManager->params(name);

    ui->cbUseColor->setChecked( def.useColor );
    ui->cbColor->setColor(def.color);
}

void EditHorizonsDialog::horizonParamsFromControls( QString name){

    if( !horizonManager ) return;

    // name and pointer are not updated!!!
    HorizonDef def;
    def.useColor = ui->cbUseColor->isChecked();
    def.color = ui->cbColor->color();

    horizonManager->setParams(name, def);

}



void EditHorizonsDialog::on_cbColor_clicked()
{

    const QColor color = QColorDialog::getColor( ui->cbColor->color(), this, "Select Horizon Color");

    if (! color.isValid()) return;

    ui->cbColor->setColor(color);

    updateParams();
}


void EditHorizonsDialog::on_cbHorizon_currentIndexChanged(const QString &arg1)
{

    horizonParamsToControls(arg1);
}
