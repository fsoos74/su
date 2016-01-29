#include "displayrangedialog.h"
#include "ui_displayrangedialog.h"

#include<QDoubleValidator>
#include <QKeyEvent>

DisplayRangeDialog::DisplayRangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayRangeDialog)
{

    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);

    ui->leMin->setValidator(validator);
    ui->leMax->setValidator(validator);
    ui->lePower->setValidator(validator);

   connect( ui->leMin, SIGNAL(returnPressed()), this, SLOT(applyRange()));
   connect( ui->leMax, SIGNAL(returnPressed()), this, SLOT(applyRange()));
   connect( ui->lePower, SIGNAL(returnPressed()), this, SLOT(applyPower()));
}

DisplayRangeDialog::~DisplayRangeDialog()
{
    delete ui;
}

std::pair<double, double> DisplayRangeDialog::range(){

    double min=ui->leMin->text().toDouble();
    double max=ui->leMax->text().toDouble();
    return std::pair<double, double>(min, max);
}

double DisplayRangeDialog::power(){

    return ui->lePower->text().toDouble();
}

void DisplayRangeDialog::setRange(std::pair<double, double> r){

    if( r==range() ) return;

    ui->leMin->setText(QString::number(r.first));
    ui->leMax->setText(QString::number(r.second));

    emit rangeChanged(r);
}

void DisplayRangeDialog::setPower( double p ){

    if( p==power()) return;

    ui->lePower->setText(QString::number(p));

    emit powerChanged(p);
}

void DisplayRangeDialog::applyRange(){

    emit rangeChanged( range());
}

void DisplayRangeDialog::applyPower(){

    emit powerChanged(power());
}

void DisplayRangeDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}
