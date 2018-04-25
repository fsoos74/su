#include "crossplotviewerdisplayoptionsdialog.h"
#include "ui_crossplotviewerdisplayoptionsdialog.h"

#include<QColorDialog>


CrossplotViewerDisplayOptionsDialog::CrossplotViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    for( auto s : {CrossplotView::Symbol::Circle, CrossplotView::Symbol::Square, CrossplotView::Symbol::Cross}){
        ui->cbSymbol->addItem(toQString(s));
    }

    connect( ui->sbPointSize, SIGNAL(valueChanged(int)),
             this, SIGNAL(pointSizeChanged(int)) );

    connect( ui->rbFixedColor, SIGNAL(toggled(bool)),
             this, SIGNAL(fixedColorChanged(bool)) );

    connect( ui->cbFixColor, SIGNAL(colorChanged(QColor)),
             this, SIGNAL(pointColorChanged(QColor)) );
}

CrossplotViewerDisplayOptionsDialog::~CrossplotViewerDisplayOptionsDialog()
{
    delete ui;
}

int CrossplotViewerDisplayOptionsDialog::pointSize(){
    return ui->sbPointSize->value();
}

bool CrossplotViewerDisplayOptionsDialog::isFixedColor(){

    return ui->rbFixedColor->isChecked();
}

QColor CrossplotViewerDisplayOptionsDialog::pointColor(){

    return ui->cbFixColor->color();
}

CrossplotView::Symbol CrossplotViewerDisplayOptionsDialog::pointSymbol(){

    return toSymbol(ui->cbSymbol->currentText());
}

QColor CrossplotViewerDisplayOptionsDialog::trendlineColor(){

    return ui->cbTrendlineColor->color();
}

void CrossplotViewerDisplayOptionsDialog::setPointSize(int size){

    // do not compare new and old value, spinbox does this anyway and we need signal to be eimitted if this is triggered by spinbox
    ui->sbPointSize->setValue(size);
}

void CrossplotViewerDisplayOptionsDialog::setFixedColor(bool on){

    ui->rbFixedColor->setChecked(on);
}

void CrossplotViewerDisplayOptionsDialog::setPointColor(QColor color){

    ui->cbFixColor->setColor(color);
}

void CrossplotViewerDisplayOptionsDialog::setPointSymbol(CrossplotView::Symbol s){
    ui->cbSymbol->setCurrentText(toQString(s));
}

void CrossplotViewerDisplayOptionsDialog::setTrendlineColor(QColor color){

    ui->cbTrendlineColor->setColor(color);
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

void CrossplotViewerDisplayOptionsDialog::on_cbSymbol_currentIndexChanged(const QString &arg1)
{
    auto s=toSymbol(arg1);
    emit pointSymbolChanged(s);
}
