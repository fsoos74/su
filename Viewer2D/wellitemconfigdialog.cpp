#include "wellitemconfigdialog.h"
#include "ui_wellitemconfigdialog.h"

#include<QPen>
#include<QBrush>
#include<QColorDialog>

/*
bool WellItemConfigDialog::configItem(WellItem *item){

    if( !item ) return false;

    WellItemConfigDialog dlg;
    dlg.setWindowTitle(tr("Configure Well Item"));
    dlg.setLabelSize(item->font().pointSize());//  pixelSize());
    dlg.setItemSize(item->size());
    dlg.setPenColor(item->pen().color());
    dlg.setBrushColor(item->brush().color());

    if( dlg.exec()!=QDialog::Accepted) return false;

    QFont f=item->font();
    //f.setPixelSize(dlg.labelSize());
    f.setPointSize(dlg.labelSize());
    item->setFont(f);

    item->setSize( dlg.itemSize() );

    QPen pen=item->pen();
    pen.setColor(dlg.penColor());
    item->setPen(pen);

    QBrush brush = item->brush();
    brush.setColor(dlg.brushColor());
    item->setBrush(brush);

    return true;
}
*/

WellItemConfigDialog::WellItemConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WellItemConfigDialog)
{
    ui->setupUi(this);
}

WellItemConfigDialog::~WellItemConfigDialog()
{
    delete ui;
}


int WellItemConfigDialog::labelType(){
    return ui->cbLabelType->currentIndex();
}

int WellItemConfigDialog::labelSize(){
        return ui->sbLabelSize->value();
}


QColor WellItemConfigDialog::symbolColor(){
    return ui->cbSymbolColor->color();
}

QColor WellItemConfigDialog::pathColor(){
    return ui->cbPathColor->color();
}

int WellItemConfigDialog::itemSize(){
    return ui->sbSize->value();
}

int WellItemConfigDialog::zValue(){
    return ui->sbZValue->value();
}

void WellItemConfigDialog::setSymbolColor(QColor c){
    ui->cbSymbolColor->setColor(c);
}

void WellItemConfigDialog::setPathColor(QColor c){
    ui->cbPathColor->setColor(c);
}

void WellItemConfigDialog::setLabelType(int i){
    ui->cbLabelType->setCurrentIndex(i);
}

void WellItemConfigDialog::setLabelSize(int i){
    ui->sbLabelSize->setValue(i);
}

void WellItemConfigDialog::setItemSize(int i){
    ui->sbSize->setValue(i);
}

void WellItemConfigDialog::setZValue(int i){
    ui->sbZValue->setValue(i);
}

void WellItemConfigDialog::on_cbSymbolColor_clicked()
{
    const QColor color = QColorDialog::getColor( ui->cbSymbolColor->color(), this, "Select Symbol Color");

    if (color.isValid()) {
      setSymbolColor(color);
    }
}

void WellItemConfigDialog::on_cbPathColor_clicked()
{
    const QColor color = QColorDialog::getColor( ui->cbPathColor->color(), this, "Select Path Color");

    if (color.isValid()) {
      setPathColor(color);
    }
}
