#include "spectrumdataselectiondialog.h"
#include "ui_spectrumdataselectiondialog.h"

#include<QIntValidator>

SpectrumDataSelectionDialog::SpectrumDataSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpectrumDataSelectionDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    ui->leIline->setValidator(validator);
    ui->leXline->setValidator(validator);
    ui->leStart->setValidator(validator);
    ui->leLen->setValidator(validator);
}

SpectrumDataSelectionDialog::~SpectrumDataSelectionDialog()
{
    delete ui;
}

QString SpectrumDataSelectionDialog::datasetName(){
    return ui->cbDataset->currentText();
}

int SpectrumDataSelectionDialog::inlineNumber(){
    return ui->leIline->text().toInt();
}

int SpectrumDataSelectionDialog::crosslineNumber(){
    return ui->leXline->text().toInt();
}

int SpectrumDataSelectionDialog::windowStartMS(){
    return ui->leStart->text().toInt();
}

int SpectrumDataSelectionDialog::windowLengthMS(){
    return ui->leLen->text().toInt();
}

void SpectrumDataSelectionDialog::setDatasetNames( const QStringList& l){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(l);
}

void SpectrumDataSelectionDialog::setInlineNumber(int i){
    ui->leIline->setText(QString::number(i));
}

void SpectrumDataSelectionDialog::setCrosslineNumber(int i){
    ui->leXline->setText(QString::number(i));
}

void SpectrumDataSelectionDialog::setWindowStartMS(int i){
    ui->leStart->setText(QString::number(i));
}

void SpectrumDataSelectionDialog::setWindowLengthMS(int i){
    ui->leLen->setText(QString::number(i));
}

void SpectrumDataSelectionDialog::on_pbAdd_clicked()
{
    SpectrumDefinition def{ datasetName(), inlineNumber(), crosslineNumber(), windowStartMS(), windowLengthMS() };
    emit dataSelected(def);
}
