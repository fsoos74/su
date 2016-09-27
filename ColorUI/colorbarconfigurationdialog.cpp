#include "colorbarconfigurationdialog.h"
#include "ui_colorbarconfigurationdialog.h"

#include<QDoubleValidator>
#include<QKeyEvent>


ColorBarConfigurationDialog::ColorBarConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorBarConfigurationDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leScaleMinimum->setValidator(validator);
    ui->leScaleMaximum->setValidator(validator);

    connect( ui->leScaleMinimum, SIGNAL(returnPressed()), this, SLOT(applyRange()));
    connect( ui->leScaleMaximum, SIGNAL(returnPressed()), this, SLOT(applyRange()));
    connect( ui->sbScaleSteps, SIGNAL(valueChanged(int)), this, SLOT(applySteps()));
}

ColorBarConfigurationDialog::~ColorBarConfigurationDialog()
{
    delete ui;
}

std::pair<double, double> ColorBarConfigurationDialog::scaleRange()const{

    return std::pair<double, double>(
                ui->leScaleMinimum->text().toDouble(), ui->leScaleMaximum->text().toDouble() );
}

int ColorBarConfigurationDialog::scaleSteps()const{

    return ui->sbScaleSteps->value();
}


void ColorBarConfigurationDialog::setScaleRange(std::pair<double, double> r){

    if( r==scaleRange() ) return;

    ui->leScaleMinimum->setText(QString::number(r.first));
    ui->leScaleMaximum->setText(QString::number(r.second));

    emit scaleRangeChanged(r);
}

void ColorBarConfigurationDialog::setScaleSteps(int n){

   ui->sbScaleSteps->setValue(n);   // spinbox will only emit event upon change so no special code necessary
}

void ColorBarConfigurationDialog::applyRange(){

    emit scaleRangeChanged(scaleRange() );
}

void ColorBarConfigurationDialog::applySteps(){

    emit scaleStepsChanged(scaleSteps());
}

void ColorBarConfigurationDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}
