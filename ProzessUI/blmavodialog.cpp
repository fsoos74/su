#include "blmavodialog.h"
#include "ui_blmavodialog.h"

#include<QIntValidator>


BLMAVODialog::BLMAVODialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::BLMAVODialog)
{
    ui->setupUi(this);

    auto validator=new QIntValidator(this);
    validator->setBottom(0);
    ui->leMaxTime->setValidator(validator);
}

BLMAVODialog::~BLMAVODialog()
{
    delete ui;
}

QString BLMAVODialog::exportPath() const{
    return ui->leExportPath->text();
}

QString BLMAVODialog::lineName() const{
    return ui->leLine->text();
}

int BLMAVODialog::maxTime() const{
    return ui->leMaxTime->text().toInt();
}

void BLMAVODialog::setExportPath(QString str){
    ui->leExportPath->setText(str);
}

void BLMAVODialog::setLineName(QString str){
    ui->leLine->setText(str);
}

void BLMAVODialog::setMaxTime(int i){
    ui->leMaxTime->setText(QString::number(i));
}

QMap<QString,QString> BLMAVODialog::params(){

    QMap<QString, QString> p;


    p.insert( "path", ui->leExportPath->text() );

    p.insert( "line", ui->leLine->text() );

    p.insert( "max-time", ui->leMaxTime->text());

    return p;
}
