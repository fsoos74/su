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
    ui->leCenter->setValidator(validator);
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

int SpectrumDataSelectionDialog::windowCenterMS(){
    return ui->leCenter->text().toInt();
}

int SpectrumDataSelectionDialog::windowSamples(){
    return ui->cbWindowSamples->currentText().toInt();
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

void SpectrumDataSelectionDialog::setWindowCenterMS(int i){
    ui->leCenter->setText(QString::number(i));
}

void SpectrumDataSelectionDialog::setWindowSamples(int i){
    ui->cbWindowSamples->setCurrentText(QString::number(i));
}

void SpectrumDataSelectionDialog::on_pbAdd_clicked()
{
    SpectrumDefinition def{ datasetName(), inlineNumber(), crosslineNumber(), windowCenterMS(), windowSamples() };
    emit dataSelected(def);
}
