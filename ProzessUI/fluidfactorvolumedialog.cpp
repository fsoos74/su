#include "fluidfactorvolumedialog.h"
#include "ui_fluidfactorvolumedialog.h"

#include<QPushButton>
#include<QDoubleValidator>
#include<avoproject.h>

FluidFactorVolumeDialog::FluidFactorVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FluidFactorVolumeDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leAngle->setValidator(validator);

    connect( ui->leGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

FluidFactorVolumeDialog::~FluidFactorVolumeDialog()
{
    delete ui;
}

void FluidFactorVolumeDialog::setInterceptVolumes( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void FluidFactorVolumeDialog::setGradientVolumes( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}


QMap<QString,QString> FluidFactorVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("volume"), ui->leGrid->text());

    p.insert( QString("intercept"), ui->cbIntercept->currentText());

    p.insert( QString("gradient"), ui->cbGradient->currentText() );

    if( !ui->cbComputeAngle->isChecked() ){
        p.insert( QString("angle"),ui->leAngle->text() );
    }

    return p;
}

void FluidFactorVolumeDialog::updateOkButton(){

    bool ok=true;

    if( ui->leGrid->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGrid->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

