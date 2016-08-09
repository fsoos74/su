#include "volumedataselectiondialog.h"
#include "ui_volumedataselectiondialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QPushButton>

VolumeDataSelectionDialog::VolumeDataSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeDataSelectionDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leMinInline->setValidator(ivalidator);
    ui->leMaxInline->setValidator(ivalidator);
    ui->leMinCrossline->setValidator(ivalidator);
    ui->leMaxCrossline->setValidator(ivalidator);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leMinTime->setValidator(dvalidator);
    ui->leMaxTime->setValidator(dvalidator);

    connect( ui->leMinInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMinCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMinTime, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxTime, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );

    connect( this, SIGNAL(areaModeChanged(bool)), ui->leMinTime, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(areaModeChanged(bool)), ui->leMaxTime, SLOT(setEnabled(bool)) );
}

VolumeDataSelectionDialog::~VolumeDataSelectionDialog()
{
    delete ui;
}

int VolumeDataSelectionDialog::minInline()const{

    return ui->leMinInline->text().toInt();
}

int VolumeDataSelectionDialog::maxInline()const{

    return ui->leMaxInline->text().toInt();
}

int VolumeDataSelectionDialog::minCrossline()const{

    return ui->leMinCrossline->text().toInt();
}

int VolumeDataSelectionDialog::maxCrossline()const{

    return ui->leMaxCrossline->text().toInt();
}

double VolumeDataSelectionDialog::minTime()const{

    return ui->leMinTime->text().toDouble();
}

double VolumeDataSelectionDialog::maxTime()const{

    return ui->leMaxTime->text().toDouble();
}

void VolumeDataSelectionDialog::setMinInline(int line){

    ui->leMinInline->setText(QString::number(line));
}

void VolumeDataSelectionDialog::setMaxInline(int line){

    ui->leMaxInline->setText(QString::number(line));
}

void VolumeDataSelectionDialog::setMinCrossline(int line){

    ui->leMinCrossline->setText(QString::number(line));
}

void VolumeDataSelectionDialog::setMaxCrossline(int line){

    ui->leMaxCrossline->setText(QString::number(line));
}

void VolumeDataSelectionDialog::setMinTime(double time){

    ui->leMinTime->setText(QString::number(time));
}

void VolumeDataSelectionDialog::setMaxTime(double time){

    ui->leMaxTime->setText(QString::number(time));
}

void VolumeDataSelectionDialog::setAreaMode(bool on){

    if( on == m_areaMode ) return;

    m_areaMode = on;

    emit areaModeChanged(m_areaMode);
}

void VolumeDataSelectionDialog::checkOk(){

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
                minInline()<=maxInline() &&
                minCrossline()<=maxCrossline() &&
                minTime()<=maxTime() );
}
