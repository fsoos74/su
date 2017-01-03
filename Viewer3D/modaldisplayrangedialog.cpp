#include "modaldisplayrangedialog.h"
#include "ui_modaldisplayrangedialog.h"

#include <QDoubleValidator>

ModalDisplayRangeDialog::ModalDisplayRangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModalDisplayRangeDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);

    ui->leMinimum->setValidator(validator);
    ui->leMaximum->setValidator(validator);
    ui->lePower->setValidator(validator);
}

ModalDisplayRangeDialog::~ModalDisplayRangeDialog()
{
    delete ui;
}


qreal ModalDisplayRangeDialog::minimum()const{

    return ui->leMinimum->text().toDouble();
}

qreal ModalDisplayRangeDialog::maximum()const{

    return ui->leMaximum->text().toDouble();
}

qreal ModalDisplayRangeDialog::power()const{

    return ui->lePower->text().toDouble();
}

void ModalDisplayRangeDialog::setMinimum(qreal m){

    if( m==minimum()) return;

    ui->leMinimum->setText(QString::number(m));
}

void ModalDisplayRangeDialog::setMaximum(qreal m){

    if( m==maximum()) return;

    ui->leMaximum->setText(QString::number(m));
}

void ModalDisplayRangeDialog::setPower(qreal m){

    if( m==power()) return;

    ui->lePower->setText(QString::number(m));
}

