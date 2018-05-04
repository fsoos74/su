#include "linedisplayoptionsdialog.h"
#include "ui_linedisplayoptionsdialog.h"

#include<QColorDialog>

LineDisplayOptionsDialog::LineDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbWidth, SIGNAL(valueChanged(int)), this, SIGNAL(lineWidthChanged(int)));
    connect( ui->sbOpacity, SIGNAL(valueChanged(int)), this, SIGNAL(opacityChanged(int)));
}

LineDisplayOptionsDialog::~LineDisplayOptionsDialog()
{
    delete ui;
}

int LineDisplayOptionsDialog::lineWidth()const{

    return ui->sbWidth->value();
}

int LineDisplayOptionsDialog::opacity()const{
    return ui->sbOpacity->value();
}

QColor LineDisplayOptionsDialog::color()const{

    return ui->cbColor->color();
}

void LineDisplayOptionsDialog::setLineWidth(int size){

    ui->sbWidth->setValue(size);
}

void LineDisplayOptionsDialog::setOpacity(int o){
    ui->sbOpacity->setValue(o);
}

void LineDisplayOptionsDialog::setColor(QColor c){

    if( c==color()) return;

    ui->cbColor->setColor(c);

    emit colorChanged(c);
}

void LineDisplayOptionsDialog::on_cbColor_clicked()
{
    const QColor c = QColorDialog::getColor( color(), this, "Select Point Color");

    if (c.isValid()) {

      setColor(c);
    }
}
