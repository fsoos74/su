#include "linedisplayoptionsdialog.h"
#include "ui_linedisplayoptionsdialog.h"

#include<QColorDialog>

LineDisplayOptionsDialog::LineDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineDisplayOptionsDialog)
{
    ui->setupUi(this);
}

LineDisplayOptionsDialog::~LineDisplayOptionsDialog()
{
    delete ui;
}

int LineDisplayOptionsDialog::pointSize()const{

    return ui->sbSize->value();
}

int LineDisplayOptionsDialog::opacity()const{
    return ui->sbOpacity->value();
}

QColor LineDisplayOptionsDialog::pointColor()const{

    return ui->cbColor->color();
}

void LineDisplayOptionsDialog::setPointSize(int size){

    ui->sbSize->setValue(size);
}

void LineDisplayOptionsDialog::setOpacity(int o){
    ui->sbOpacity->setValue(o);
}

void LineDisplayOptionsDialog::setPointColor(QColor color){

    if( color==pointColor()) return;

    ui->cbColor->setColor(color);

    emit pointColorChanged(color);
}

void LineDisplayOptionsDialog::on_cbColor_clicked()
{
    const QColor color = QColorDialog::getColor( pointColor(), this, "Select Point Color");

    if (color.isValid()) {

      setPointColor(color);
    }
}

void LineDisplayOptionsDialog::on_sbSize_valueChanged(int size)
{
    emit pointSizeChanged(size);
}

void LineDisplayOptionsDialog::on_sbOpacity_valueChanged(int arg1)
{
    emit opacityChanged(arg1);
}
