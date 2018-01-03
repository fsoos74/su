#include "crossplotviewerdisplayoptionsdialog.h"
#include "ui_crossplotviewerdisplayoptionsdialog.h"

#include<QColorDialog>

CrossplotViewerDisplayOptionsDialog::CrossplotViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbDatapointSize, SIGNAL(valueChanged(int)),
             this, SLOT(setDatapointSize(int)) );

    connect( ui->rbFixedColor, SIGNAL(toggled(bool)),
             this, SLOT(setFixedColor(bool)) );

    connect( ui->cbFixColor, SIGNAL(colorChanged(QColor)),
             this, SLOT(setPointColor(QColor)) );
}

CrossplotViewerDisplayOptionsDialog::~CrossplotViewerDisplayOptionsDialog()
{
    delete ui;
}

int CrossplotViewerDisplayOptionsDialog::datapointSize(){
    return ui->sbDatapointSize->value();
}

bool CrossplotViewerDisplayOptionsDialog::isFixedColor(){

    return ui->rbFixedColor->isChecked();
}

QColor CrossplotViewerDisplayOptionsDialog::pointColor(){

    return ui->cbFixColor->color();
}

QColor CrossplotViewerDisplayOptionsDialog::trendlineColor(){

    return ui->cbTrendlineColor->color();
}

void CrossplotViewerDisplayOptionsDialog::setDatapointSize(int size){

    // do not compare new and old value, spinbox does this anyway and we need signal to be eimitted if this is triggered by spinbox
    ui->sbDatapointSize->setValue(size);

    emit datapointSizeChanged(size);
}

void CrossplotViewerDisplayOptionsDialog::setFixedColor(bool on){

     // do not compare new and old value, we need signal to be eimitted

    ui->rbFixedColor->setChecked(on);

    emit fixedColorChanged(on);
}

void CrossplotViewerDisplayOptionsDialog::setPointColor(QColor color){

    // do not compare new and old value, we need signal to be eimitted

    ui->cbFixColor->setColor(color);

    emit pointColorChanged(color);
}

void CrossplotViewerDisplayOptionsDialog::setTrendlineColor(QColor color){

    // do not compare new and old value, we need signal to be eimitted

    ui->cbTrendlineColor->setColor(color);

    emit trendlineColorChanged(color);
}


void CrossplotViewerDisplayOptionsDialog::on_cbFixColor_clicked()
{
    const QColor color = QColorDialog::getColor( pointColor(), this, "Select Datapoint Color");

    if (color.isValid()) {

        setPointColor(color);
    }
}

void CrossplotViewerDisplayOptionsDialog::on_cbTrendlineColor_clicked()
{
    const QColor color = QColorDialog::getColor( trendlineColor(), this,
                                                 "Select Trendline Color");

    if (color.isValid()) {

        setTrendlineColor(color);
    }
}
