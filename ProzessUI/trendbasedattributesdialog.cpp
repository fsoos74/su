#include "trendbasedattributesdialog.h"
#include "ui_trendbasedattributesdialog.h"

#include<avoproject.h>
#include<QPushButton>
#include<QDoubleValidator>


TrendBasedAttributesDialog::TrendBasedAttributesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::TrendBasedAttributesDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leTrendAngle->setValidator(validator);
    ui->leTrendIntercept->setValidator(validator);
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

TrendBasedAttributesDialog::~TrendBasedAttributesDialog()
{
    delete ui;
}

void TrendBasedAttributesDialog::setInterceptList( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void TrendBasedAttributesDialog::setGradientList( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}

void TrendBasedAttributesDialog::setComputeTrend(bool on){
    ui->cbComputeTrend->setChecked(on);
}

void TrendBasedAttributesDialog::setTrendIntercept(double a){
    ui->leTrendIntercept->setText(QString::number(a));
}

void TrendBasedAttributesDialog::setTrendAngle(double a){
    ui->leTrendAngle->setText(QString::number(a));
}

void TrendBasedAttributesDialog::setVolumeMode(bool on){
    m_volumeMode=on;
    updateOkButton();
}

QMap<QString,QString> TrendBasedAttributesDialog::params(){

    QMap<QString, QString> p;

    p.insert(QString("type"), QString::number(ui->cbType->currentIndex()));

    p.insert(QString("intercept"), ui->cbIntercept->currentText());

    p.insert(QString("gradient"), ui->cbGradient->currentText());

    p.insert(QString("compute-trend"), QString::number(ui->cbComputeTrend->isChecked() ) );

    if( !ui->cbComputeTrend->isChecked() ){
        p.insert(QString("trend-angle"), ui->leTrendAngle->text() );
        p.insert(QString("trend-intercept"), ui->leTrendIntercept->text() );
    }

    QString fullGridName=(m_volumeMode) ? ui->leOutput->text() : createFullGridName( GridType::Attribute, ui->leOutput->text() );
    p.insert(QString("output"), fullGridName );

    return p;
}


void TrendBasedAttributesDialog::updateOkButton(){

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
