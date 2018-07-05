#include "trendbasedattributevolumesdialog.h"
#include "ui_trendbasedattributevolumesdialog.h"

#include<avoproject.h>
#include<QPushButton>
#include<QDoubleValidator>


TrendBasedAttributeVolumesDialog::TrendBasedAttributeVolumesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::TrendBasedAttributeVolumesDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leTrendAngle->setValidator(validator);
    ui->leTrendIntercept->setValidator(validator);
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

TrendBasedAttributeVolumesDialog::~TrendBasedAttributeVolumesDialog()
{
    delete ui;
}

void TrendBasedAttributeVolumesDialog::setInterceptList( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void TrendBasedAttributeVolumesDialog::setGradientList( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}

void TrendBasedAttributeVolumesDialog::setHorizons(const QStringList & list){
    ui->cbTop->clear();
    ui->cbTop->addItems(list);
    ui->cbBottom->clear();
    ui->cbBottom->addItems(list);
}

void TrendBasedAttributeVolumesDialog::setComputeTrend(bool on){
    ui->cbComputeTrend->setChecked(on);
}

void TrendBasedAttributeVolumesDialog::setTrendIntercept(double a){
    ui->leTrendIntercept->setText(QString::number(a));
}

void TrendBasedAttributeVolumesDialog::setTrendAngle(double a){
    ui->leTrendAngle->setText(QString::number(a));
}

QMap<QString,QString> TrendBasedAttributeVolumesDialog::params(){

    QMap<QString, QString> p;

    p.insert("attribute", QString::number(ui->cbType->currentIndex()));
    p.insert("intercept", ui->cbIntercept->currentText());
    p.insert("gradient", ui->cbGradient->currentText());
    bool selectLayer=ui->cbSelectLayer->isChecked();
    p.insert("select-layer", QString::number( selectLayer ) );
    p.insert("top-horizon", (selectLayer) ? ui->cbTop->currentText() : "");
    p.insert("bottom-horizon", (selectLayer) ? ui->cbBottom->currentText() : "");
    bool computeTrend=ui->cbComputeTrend->isChecked();
    p.insert("compute-trend", QString::number(computeTrend ) );
    p.insert("trend-angle", (computeTrend) ? ui->leTrendAngle->text() : "" );
    p.insert("trend-intercept", (computeTrend) ? ui->leTrendIntercept->text() : "" );
    p.insert(QString("output"), ui->leOutput->text() );

    return p;
}


void TrendBasedAttributeVolumesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedVolumes().contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);
    }

    {
    QPalette palette;
    if(ui->cbSelectLayer->isChecked() && ui->cbTop->currentText()==ui->cbBottom->currentText()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->cbTop->setPalette(palette);
    ui->cbBottom->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
