#include "orientationdialog.h"
#include "ui_orientationdialog.h"
#include<iostream>


const QString strHorizontal("Horizontal");
const QString strVertical("Vertical");
const QString strAscending("Ascending");
const QString strDescending("Descending");

const int indexHorizontal=0;
const int indexVertical=1;
const int indexAscending=0;
const int indexDescending=1;

OrientationDialog::OrientationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrientationDialog)
{
    ui->setupUi(this);

    ui->cbInlineOrientation->addItem(strHorizontal);
    ui->cbInlineOrientation->addItem(strVertical);

    ui->cbInlineDirection->addItem(strAscending);
    ui->cbInlineDirection->addItem(strDescending);

    ui->cbCrosslineDirection->addItem(strAscending);
    ui->cbCrosslineDirection->addItem(strDescending);
}

OrientationDialog::~OrientationDialog()
{
    delete ui;
}


AxxisOrientation OrientationDialog::inlineOrientation()const{

    switch( ui->cbInlineOrientation->currentIndex() ){

    case indexHorizontal: return AxxisOrientation::Horizontal; break;
    case indexVertical: return AxxisOrientation::Vertical; break;
    default:
        qFatal("There could possibly never be an invalid axxis orientation!");
        break;
    }
}


AxxisDirection OrientationDialog::inlineDirection()const{

    if( ui->cbInlineDirection->currentIndex()==indexAscending ){
        return AxxisDirection::Ascending;
    }
    else if( ui->cbInlineDirection->currentIndex()==indexDescending){
        return AxxisDirection::Descending;
    }
    else{
        qFatal("I was sure there would never be an invalid axxis direction!");
    }
}



AxxisDirection OrientationDialog::crosslineDirection()const{

    if( ui->cbCrosslineDirection->currentIndex()==indexAscending ){
        return AxxisDirection::Ascending;
    }
    else if( ui->cbCrosslineDirection->currentIndex()==indexDescending){
        return AxxisDirection::Descending;
    }
    else{
        qFatal("I was sure there would never be an invalid axxis direction!");
    }
}


void OrientationDialog::setInlineOrientation( AxxisOrientation d){

    if( d==inlineOrientation()) return;

    if( d==AxxisOrientation::Horizontal){
        ui->cbInlineOrientation->setCurrentIndex(indexHorizontal);
    }
    else if( d==AxxisOrientation::Vertical){
        ui->cbInlineOrientation->setCurrentIndex(indexVertical);
    }
    else{
        qFatal("Screwed up on inline orientation!!!");
    }

    emit inlineOrientationChanged(d);
}

void OrientationDialog::setInlineDirection( AxxisDirection d){

    if( d==inlineDirection()) return;

    if( d==AxxisDirection::Ascending){
        ui->cbInlineDirection->setCurrentIndex(indexAscending);
    }
    else if( d==AxxisDirection::Descending){
        ui->cbInlineDirection->setCurrentIndex(indexDescending);
    }
    else{
        qFatal("Screwed up on inline direction!");
    }

    emit inlineDirectionChanged(d);
}

void OrientationDialog::setCrosslineDirection( AxxisDirection d){

    if( d==crosslineDirection()) return;

    if( d==AxxisDirection::Ascending){
        ui->cbCrosslineDirection->setCurrentIndex(indexAscending);
    }
    else if( d==AxxisDirection::Descending){
        ui->cbCrosslineDirection->setCurrentIndex(indexDescending);
    }
    else{
        qFatal("Screwed up on crossline direction!");
    }

    emit crosslineDirectionChanged(d);
}

void OrientationDialog::setOrientationParams( AxxisOrientation ilOrientation, AxxisDirection ilDirection, AxxisDirection xlDirection){

    setInlineOrientation( ilOrientation);
    setInlineDirection(ilDirection);
    setCrosslineDirection(xlDirection);
}

void OrientationDialog::on_cbInlineOrientation_currentIndexChanged(int)
{
    emit inlineOrientationChanged(inlineOrientation());
}

void OrientationDialog::on_cbInlineDirection_currentIndexChanged(int)
{
    emit inlineDirectionChanged(inlineDirection());
}


void OrientationDialog::on_cbCrosslineDirection_currentIndexChanged(int)
{
    emit crosslineDirectionChanged(crosslineDirection());
}
