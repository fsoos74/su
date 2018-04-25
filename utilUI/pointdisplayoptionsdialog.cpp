#include "pointdisplayoptionsdialog.h"
#include "ui_pointdisplayoptionsdialog.h"

#include<QColorDialog>

PointDisplayOptionsDialog::PointDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointDisplayOptionsDialog)
{
    ui->setupUi(this);
}

PointDisplayOptionsDialog::~PointDisplayOptionsDialog()
{
    delete ui;
}

int PointDisplayOptionsDialog::pointSize()const{

    return ui->sbSize->value();
}

int PointDisplayOptionsDialog::opacity()const{
    return ui->sbOpacity->value();
}

QColor PointDisplayOptionsDialog::pointColor()const{

    return ui->cbColor->color();
}

void PointDisplayOptionsDialog::setPointSize(int size){

    ui->sbSize->setValue(size);
}

void PointDisplayOptionsDialog::setOpacity(int o){
    ui->sbOpacity->setValue(o);
}

void PointDisplayOptionsDialog::setPointColor(QColor color){

    if( color==pointColor()) return;

    ui->cbColor->setColor(color);

    emit pointColorChanged(color);
}

void PointDisplayOptionsDialog::on_cbColor_clicked()
{
    const QColor color = QColorDialog::getColor( pointColor(), this, "Select Point Color");

    if (color.isValid()) {

      setPointColor(color);
    }
}

void PointDisplayOptionsDialog::on_sbSize_valueChanged(int size)
{
    emit pointSizeChanged(size);
}

void PointDisplayOptionsDialog::on_sbOpacity_valueChanged(int arg1)
{
    emit opacityChanged(arg1);
}
