#include "griditemconfigdialog.h"
#include "ui_griditemconfigdialog.h"

#include<QDoubleValidator>

#include<colortabledialog.h>
#include <histogramrangeselectiondialog.h>

GridItemConfigDialog::GridItemConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridItemConfigDialog)
{
    ui->setupUi(this);
/*
    auto valid = new QDoubleValidator(this);
    ui->leMin->setValidator(valid);
    ui->leMax->setValidator(valid);
    ui->lePower->setValidator(valid);
    */

    connect( ui->cbMesh, SIGNAL(toggled(bool)), this, SIGNAL(showMeshChanged(bool)) );
    connect( ui->cbLabels, SIGNAL(toggled(bool)), this, SIGNAL(showLabelsChanged(bool)) );
    connect( ui->cbLabel, SIGNAL(toggled(bool)), this, SIGNAL(showLabelChanged(bool)) );
    connect(ui->leLabel, SIGNAL(textChanged(QString)), this, SIGNAL(labelTextChanged(QString)));
    connect( ui->sbInlineIncrement, SIGNAL(valueChanged(int)), this, SIGNAL(inlineIncrementChanged(int)) );
    connect( ui->sbCrosslineIncrement, SIGNAL(valueChanged(int)), this, SIGNAL(crosslineIncrementChanged(int)) );
    connect( ui->sbZValue, SIGNAL(valueChanged(int)), this, SIGNAL(zValueChanged(int)) );
}

GridItemConfigDialog::~GridItemConfigDialog()
{
    delete ui;
}

ColorTable* GridItemConfigDialog::colorTable(){
    return m_colorTable;
}

Histogram GridItemConfigDialog::histogram(){
    return m_histogram;
}

bool GridItemConfigDialog::showMesh(){
    return ui->cbMesh->isChecked();
}

bool GridItemConfigDialog::showLabels(){
    return ui->cbLabels->isChecked();
}

bool GridItemConfigDialog::showLabel(){
    return ui->cbLabel->isChecked();
}

QString GridItemConfigDialog::labelText(){
    return ui->leLabel->text();
}

int GridItemConfigDialog::inlineIncrement(){
    return ui->sbInlineIncrement->value();
}

int GridItemConfigDialog::crosslineIncrement(){
    return ui->sbCrosslineIncrement->value();
}

int GridItemConfigDialog::zValue(){
    return ui->sbZValue->value();
}

void GridItemConfigDialog::setColorTable(ColorTable* ct){
    m_colorTable=ct;
}

void GridItemConfigDialog::setHistogram(Histogram h){
    m_histogram=h;
}

void GridItemConfigDialog::setShowMesh(bool on){
    ui->cbMesh->setChecked(on);
}

void GridItemConfigDialog::setShowLabels(bool on){
    ui->cbLabels->setChecked(on);
}

void GridItemConfigDialog::setShowLabel(bool on){
    ui->cbLabel->setChecked(on);
}

void GridItemConfigDialog::setLabelText(QString t){
    ui->leLabel->setText(t);
}

void GridItemConfigDialog::setInlineIncrement(int i){
    ui->sbInlineIncrement->setValue(i);
}

void GridItemConfigDialog::setCrosslineIncrement(int i){
    ui->sbCrosslineIncrement->setValue(i);
}

void GridItemConfigDialog::setZValue(int i){
    ui->sbZValue->setValue(i);
}

void GridItemConfigDialog::on_pbScaling_clicked()
{
    if( !m_colorTable) return;

    auto displayRangeDialog=new HistogramRangeSelectionDialog(this);
    displayRangeDialog->setHistogram(m_histogram);
    displayRangeDialog->setRange(m_colorTable->range());
    displayRangeDialog->setColorTable(m_colorTable);   // all updating through colortable
    displayRangeDialog->setWindowTitle("Configure Display Range" );

    displayRangeDialog->exec();//show();
}

void GridItemConfigDialog::on_pbColortable_clicked()
{
    if( !m_colorTable) return;

    auto colors=m_colorTable->colors();
    ColorTableDialog dlg( colors, this);

    if( dlg.exec()==QDialog::Accepted){
        m_colorTable->setColors(dlg.colors());
    }
    else{
        m_colorTable->setColors(colors);
    }
}


