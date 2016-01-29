#include "griddisplayoptionsdialog.h"
#include "ui_griddisplayoptionsdialog.h"

#include<QColorDialog>


GridDisplayOptionsDialog::GridDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridDisplayOptionsDialog)
{
    ui->setupUi(this);
}

GridDisplayOptionsDialog::~GridDisplayOptionsDialog()
{
    delete ui;
}


Qt::TransformationMode GridDisplayOptionsDialog::transformationMode()const{

    switch( ui->cbQuality->currentIndex()){
    case 0: return Qt::FastTransformation; break;
    case 1: return Qt::SmoothTransformation; break;
    default: qFatal("Unexpected tranformation mode!"); break;
    }
}

QColor GridDisplayOptionsDialog::backgroundColor()const{

    return ui->cbBackgroundColor->color();
}

int GridDisplayOptionsDialog::highlightedCDPSize()const{

    return ui->sbHighlightedCDPs->value();
}

QColor GridDisplayOptionsDialog::highlightedCDPColor()const{

    return ui->cbHighlightedCDPs->color();
}

void GridDisplayOptionsDialog::setTransformationMode( Qt::TransformationMode m){

    if( m==transformationMode()) return;

    if( m==Qt::FastTransformation ){
        ui->cbQuality->setCurrentIndex(0);
    }
    else if(m==Qt::SmoothTransformation){
        ui->cbQuality->setCurrentIndex(1);
    }
    else{
        qFatal("Illegal transformation mode!");
    }

    emit transformationModeChanged(m);
}

void GridDisplayOptionsDialog::setBackgroundColor( QColor color){

    if( color==backgroundColor()) return;

    ui->cbBackgroundColor->setColor(color);

    emit backgroundColorChanged(color);
}

void GridDisplayOptionsDialog::setHighlightedCDPSize(int size){

    ui->sbHighlightedCDPs->setValue(size);
}

void GridDisplayOptionsDialog::setHighlightedCDPColor(QColor color){

    if( color==highlightedCDPColor()) return;

    ui->cbHighlightedCDPs->setColor(color);

    emit highlightedCDPColorChanged(color);
}


void GridDisplayOptionsDialog::on_cbBackgroundColor_clicked()
{
    const QColor color = QColorDialog::getColor( backgroundColor(), this, "Select Background Color");


    if (color.isValid()) {

      setBackgroundColor(color);

    }
}

void GridDisplayOptionsDialog::on_cbQuality_currentIndexChanged(int)
{
    emit transformationModeChanged( transformationMode());
}

void GridDisplayOptionsDialog::on_cbHighlightedCDPs_clicked()
{
    const QColor color = QColorDialog::getColor( highlightedCDPColor(), this, "Select Highlighted CDP Color");

    if (color.isValid()) {

      setHighlightedCDPColor(color);
    }
}

void GridDisplayOptionsDialog::on_sbHighlightedCDPs_valueChanged(int size)
{
    emit highlightedCDPSizeChanged(size);
}
