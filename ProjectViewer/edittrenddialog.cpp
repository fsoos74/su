#include "edittrenddialog.h"
#include "ui_edittrenddialog.h"

#include<QDoubleValidator>

EditTrendDialog::EditTrendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTrendDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leAngle->setValidator(validator);
    ui->leIntercept->setValidator(validator);
}

EditTrendDialog::~EditTrendDialog()
{
    delete ui;
}

double EditTrendDialog::angle(){
    return ui->leAngle->text().toDouble();
}

double EditTrendDialog::intercept(){
    return ui->leIntercept->text().toDouble();
}

void EditTrendDialog::setAngle(double x){
    ui->leAngle->setText(QString::number(x));
}

void EditTrendDialog::setIntercept(double x){
    ui->leIntercept->setText(QString::number(x));
}
