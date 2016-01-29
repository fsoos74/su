#include "tracescalingdialog.h"
#include "ui_tracescalingdialog.h"

TraceScalingDialog::TraceScalingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TraceScalingDialog)
{
    ui->setupUi(this);


    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leScalingFactor->setValidator(validator);
}

TraceScalingDialog::~TraceScalingDialog()
{
    delete ui;
}

void TraceScalingDialog::setScalingMode( GatherScaler::Mode mode){

    switch( mode ){

    case GatherScaler::Mode::NoScaling:
        ui->cbScalingMode->setCurrentIndex(0);
        break;
    case GatherScaler::Mode::Fixed:
        ui->cbScalingMode->setCurrentIndex(1);
        break;
    case GatherScaler::Mode::Individual:
        ui->cbScalingMode->setCurrentIndex(2);
        break;
    case GatherScaler::Mode::Entire:
        ui->cbScalingMode->setCurrentIndex(3);
        break;
    default:
        qFatal("this should never happen!");
        break;
    }
}


void TraceScalingDialog::setScalingFactor(qreal factor){

    ui->leScalingFactor->setText(QString::number(factor));
}

void TraceScalingDialog::on_cbScalingMode_currentIndexChanged(int idx){

    GatherScaler::Mode mode;

    switch( idx){
    case 0:
        mode=GatherScaler::Mode::NoScaling;
        break;
    case 1:
        mode=GatherScaler::Mode::Fixed;
        break;
    case 2:
        mode=GatherScaler::Mode::Individual;
        break;
    case 3:
        mode=GatherScaler::Mode::Entire;
        break;
    default:
        qFatal("This should never happen!");
        break;
    }

    emit scalingModeChanged(mode);
}


void TraceScalingDialog::on_leScalingFactor_returnPressed()
{
    qreal factor=ui->leScalingFactor->text().toDouble();

    emit scalingFactorChanged(factor);
}
