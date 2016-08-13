#include "fluidfactordialog.h"
#include "ui_fluidfactordialog.h"

#include<QPushButton>
#include<QDoubleValidator>
#include<avoproject.h>

FluidFactorDialog::FluidFactorDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FluidFactorDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leAngle->setValidator(validator);

    connect( ui->leGrid, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

FluidFactorDialog::~FluidFactorDialog()
{
    delete ui;
}

void FluidFactorDialog::setInterceptGrids( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void FluidFactorDialog::setGradientGrids( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}


QMap<QString,QString> FluidFactorDialog::params(){

    QMap<QString, QString> p;

    QString fullGridName=createFullGridName( GridType::Attribute, ui->leGrid->text() );

    p.insert( QString("grid"), fullGridName);

    p.insert( QString("intercept"), ui->cbIntercept->currentText());

    p.insert( QString("gradient"), ui->cbGradient->currentText() );

    if( !ui->cbComputeAngle->isChecked() ){
        p.insert( QString("angle"),ui->leAngle->text() );
    }

    if( ui->cbDisplay->isChecked()){
        p.insert( QString("display-grid"), fullGridName);
    }

    return p;
}

void FluidFactorDialog::updateOkButton(){

    bool ok=true;

    if( ui->leGrid->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedGrids().contains(ui->leGrid->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leGrid->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

