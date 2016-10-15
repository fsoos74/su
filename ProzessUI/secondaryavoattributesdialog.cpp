#include "secondaryavoattributesdialog.h"
#include "ui_secondaryavoattributesdialog.h"

#include<avoproject.h>
#include<QPushButton>

SecondaryAVOAttributesDialog::SecondaryAVOAttributesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::SecondaryAVOAttributesDialog)
{
    ui->setupUi(this);

    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

SecondaryAVOAttributesDialog::~SecondaryAVOAttributesDialog()
{
    delete ui;
}

void SecondaryAVOAttributesDialog::setInterceptList( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void SecondaryAVOAttributesDialog::setGradientList( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}

void SecondaryAVOAttributesDialog::setFluidFactorList( const QStringList& h){
    ui->cbFluidFactor->clear();
    ui->cbFluidFactor->addItems(h);
}

void SecondaryAVOAttributesDialog::setVolumeMode(bool on){
    m_volumeMode=on;
    updateOkButton();
}

QMap<QString,QString> SecondaryAVOAttributesDialog::params(){

    QMap<QString, QString> p;

    p.insert(QString("type"), QString::number(ui->cbType->currentIndex()));

    p.insert(QString("intercept"), ui->cbIntercept->currentText());

    p.insert(QString("gradient"), ui->cbGradient->currentText());

    if( ui->stackedWidget->currentIndex()==TypeIndex::GxIxFF){
        p.insert(QString("fluid-factor"), ui->cbFluidFactor->currentText());
    }

    QString fullGridName=createFullGridName( GridType::Attribute, ui->leOutput->text() );
    p.insert(QString("output"), fullGridName );

    return p;
}


void SecondaryAVOAttributesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( ( isVolumeMode() && reservedVolumes().contains(ui->leOutput->text())
          || reservedGrids().contains(ui->leOutput->text()) )){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
