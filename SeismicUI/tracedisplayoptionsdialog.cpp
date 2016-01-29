#include "tracedisplayoptionsdialog.h"
#include "ui_tracedisplayoptionsdialog.h"

#include<QKeyEvent>
#include<QColorDialog>
#include<colortable.h>


TraceDisplayOptionsDialog::TraceDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TraceDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbTraceOpacity, SIGNAL(valueChanged(int)), this, SLOT(setTraceOpacity(int)) );
    connect( ui->sbDensityOpacity, SIGNAL(valueChanged(int)), this, SLOT(setDensityOpacity(int)) );
}

TraceDisplayOptionsDialog::~TraceDisplayOptionsDialog()
{
    delete ui;
}

bool TraceDisplayOptionsDialog::isDisplayWiggles(){
    return ui->cbWiggles->isChecked();
}

bool TraceDisplayOptionsDialog::isDisplayVariableArea(){
    return ui->cbVariableArea->isChecked();
}

bool TraceDisplayOptionsDialog::isDisplayDensity(){
    return ui->cbDensity->isChecked();
}

QColor TraceDisplayOptionsDialog::traceColor(){
    return ui->cbTraceColor->color();
}

int TraceDisplayOptionsDialog::traceOpacity(){
    return ui->sbTraceOpacity->value();
}

int TraceDisplayOptionsDialog::densityOpacity(){
    return ui->sbDensityOpacity->value();
}

void TraceDisplayOptionsDialog::setDisplayWiggles( bool on ){
    ui->cbWiggles->setChecked(on);
}

void TraceDisplayOptionsDialog::setDisplayVariableArea( bool on){
    ui->cbVariableArea->setChecked(on);
}

void TraceDisplayOptionsDialog::setDisplayDensity( bool on){
    ui->cbDensity->setChecked(on);
}



void TraceDisplayOptionsDialog::setColors(QVector<QRgb> cols){

    if( cols==m_colors) return;
    m_colors=cols;
    emit colorsChanged(cols);
}

void TraceDisplayOptionsDialog::setTraceColor(QColor rgb){

    ui->cbTraceColor->setColor(rgb);
    emit traceColorChanged(rgb);
}

void TraceDisplayOptionsDialog::setTraceOpacity(int i){
    // no check if value is equal, this is done be spinbox
    ui->sbTraceOpacity->setValue(i);
    emit traceOpacityChanged(i);
}

void TraceDisplayOptionsDialog::setDensityOpacity(int i){
    // no check if value is equal, this is done be spinbox
    ui->sbDensityOpacity->setValue(i);
    emit densityOpacityChanged(i);
}

void TraceDisplayOptionsDialog::keyPressEvent(QKeyEvent *ev){

    if( ev->key() == Qt::Key_Return){
        ev->accept(); // ate it
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}


void TraceDisplayOptionsDialog::on_pbColorTable_clicked()
{
    QVector<QRgb> oldColors=m_colors;

    ColorTableDialog* dlg=new ColorTableDialog( oldColors);
    connect( dlg, SIGNAL(colorsChanged(QVector<QRgb>)), this, SLOT(setColors(QVector<QRgb>)) );

    if( dlg->exec()==QDialog::Accepted ){
        setColors(dlg->colors());
    }
    else{
        setColors(oldColors);
    }

    delete dlg;
}

void TraceDisplayOptionsDialog::on_cbWiggles_toggled(bool checked)
{
    emit displayWigglesChanged(checked);
}

void TraceDisplayOptionsDialog::on_cbVariableArea_toggled(bool checked)
{
    emit displayVariableAreaChanged(checked);
}

void TraceDisplayOptionsDialog::on_cbDensity_toggled(bool checked)
{
    emit displayDensityChanged(checked);
}

void TraceDisplayOptionsDialog::on_cbTraceColor_clicked()
{
    const QColor color = QColorDialog::getColor( traceColor(), this, "Select Background Color");


    if (color.isValid()) {

        setTraceColor(color);
    }
}

void TraceDisplayOptionsDialog::on_sbTraceOpacity_valueChanged(int arg1)
{
    setTraceOpacity(arg1);
}
