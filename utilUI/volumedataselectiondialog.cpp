#include "volumedataselectiondialog.h"
#include "ui_volumedataselectiondialog.h"

#include<QIntValidator>
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
    ui->leMinMSec->setValidator(ivalidator);
    ui->leMaxMSec->setValidator(ivalidator);

    connect( ui->leMinInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMinCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMinMSec, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxMSec, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );

    connect( this, SIGNAL(areaModeChanged(bool)), ui->leMinMSec, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(areaModeChanged(bool)), ui->leMaxMSec, SLOT(setEnabled(bool)) );
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

int VolumeDataSelectionDialog::minMSec()const{

    return ui->leMinMSec->text().toInt();
}

int VolumeDataSelectionDialog::maxMSec()const{

    return ui->leMaxMSec->text().toInt();
}

VolumeDimensions VolumeDataSelectionDialog::dimensions()const{

    return VolumeDimensions( minInline(), maxInline(), minCrossline(), maxCrossline(), minMSec(), maxMSec() );
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

void VolumeDataSelectionDialog::setMinMSec(int time){

    ui->leMinMSec->setText(QString::number(time));
}

void VolumeDataSelectionDialog::setMaxMSec(int time){

    ui->leMaxMSec->setText(QString::number(time));
}

void VolumeDataSelectionDialog::setDimensions(VolumeDimensions dim){

    setMinInline(dim.inline1);
    setMaxInline(dim.inline2);
    setMinCrossline(dim.crossline1);
    setMaxCrossline(dim.crossline2);
    setMinMSec(dim.msec1);
    setMaxMSec(dim.msec2);
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
                minMSec()<=maxMSec() );
}
