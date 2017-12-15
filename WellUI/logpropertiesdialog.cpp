#include "logpropertiesdialog.h"
#include "ui_logpropertiesdialog.h"

LogPropertiesDialog::LogPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogPropertiesDialog)
{
    ui->setupUi(this);
}

LogPropertiesDialog::~LogPropertiesDialog()
{
    delete ui;
}


QString LogPropertiesDialog::name(){
    return ui->leName->text();
}


QString LogPropertiesDialog::unit(){
    return ui->leUnit->text();
}


QString LogPropertiesDialog::description(){
    return ui->teDescr->toPlainText();
}


void LogPropertiesDialog::setName(QString s){
    ui->leName->setText(s);
}


void LogPropertiesDialog::setUnit(QString s){
    ui->leUnit->setText(s);
}


void LogPropertiesDialog::setDescription(QString s){
    ui->teDescr->setText(s);
}
