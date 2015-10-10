#include "displayrangedialog.h"
#include "ui_displayrangedialog.h"

#include<QDoubleValidator>

DisplayRangeDialog::DisplayRangeDialog(std::pair<double, double> range, QWidget *parent) :
    QDialog(parent),
    m_startRange(range),
    m_range( range),
    ui(new Ui::DisplayRangeDialog)
{
    ui->setupUi(this);

    m_validator=new QDoubleValidator(this);

    ui->leMin->setValidator(m_validator);
    ui->leMax->setValidator(m_validator);

    updateMaxControl();
    updateMinControl();


}

DisplayRangeDialog::~DisplayRangeDialog()
{
    delete ui;
}


void DisplayRangeDialog::setRange(std::pair<double, double> r){

    if( r==m_range ) return;

    m_range=r;

    updateMinControl();
    updateMaxControl();

    emit displayRangeChanged(m_range);
}

void DisplayRangeDialog::on_pbMin_clicked()
{
    m_range.first=m_startRange.first;
    updateMinControl();
    emit displayRangeChanged(m_range);
}

void DisplayRangeDialog::on_pbMax_clicked()
{
    m_range.second=m_startRange.second;
    updateMaxControl();
    emit displayRangeChanged(m_range);
}


void DisplayRangeDialog::updateMinControl(){
    ui->leMin->setText(QString::number(m_range.first));
}

void DisplayRangeDialog::updateMinFromControl(){
    m_range.first=ui->leMin->text().toDouble();
}

void DisplayRangeDialog::updateMaxControl(){
    ui->leMax->setText(QString::number(m_range.second));
}

void DisplayRangeDialog::updateMaxFromControl(){
    m_range.second=ui->leMax->text().toDouble();
}



void DisplayRangeDialog::on_pbApply_clicked()
{
    updateMinFromControl();
    updateMaxFromControl();
    emit displayRangeChanged(m_range);
}

void DisplayRangeDialog::on_pbOk_clicked()
{
    updateMinFromControl();
    updateMaxFromControl();
    accept();
}
