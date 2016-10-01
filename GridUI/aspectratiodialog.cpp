#include "aspectratiodialog.h"
#include "ui_aspectratiodialog.h"

#include <QDoubleValidator>
#include<QKeyEvent>

AspectRatioDialog::AspectRatioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AspectRatioDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    validator->setBottom(0);
    ui->leRatio->setValidator(validator);
}

AspectRatioDialog::~AspectRatioDialog()
{
    delete ui;
}

double AspectRatioDialog::ratio(){
    return ui->leRatio->text().toDouble();
}

bool AspectRatioDialog::isFixed(){
    return ui->cbFixed->isChecked();
}

void AspectRatioDialog::setDefaultRatio(double r){
    m_defaultRatio=r;
}

void AspectRatioDialog::setRatio(double r){

    if( r==ratio()) return;

    ui->leRatio->setText(QString::number(r));

    emit ratioChanged(r);
}

void AspectRatioDialog::setFixed(bool on){
    ui->cbFixed->setChecked(on);    // this will emit toggled if changed
}

void AspectRatioDialog::on_pbDefault_clicked()
{
    setRatio(m_defaultRatio);
}

void AspectRatioDialog::on_cbFixed_toggled(bool checked)
{
    emit fixedChanged(checked);
}

void AspectRatioDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}


void AspectRatioDialog::on_leRatio_returnPressed()
{
    emit ratioChanged( ratio() );
}
