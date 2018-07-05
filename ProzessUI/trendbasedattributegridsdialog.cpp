#include "trendbasedattributegridsdialog.h"
#include "ui_trendbasedattributegridsdialog.h"

#include<avoproject.h>
#include<QPushButton>
#include<QDoubleValidator>


TrendBasedAttributeGridsDialog::TrendBasedAttributeGridsDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::TrendBasedAttributeGridsDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leTrendAngle->setValidator(validator);
    ui->leTrendIntercept->setValidator(validator);
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

TrendBasedAttributeGridsDialog::~TrendBasedAttributeGridsDialog()
{
    delete ui;
}

void TrendBasedAttributeGridsDialog::setInterceptList( const QStringList& h){
    ui->cbIntercept->clear();
    ui->cbIntercept->addItems(h);
}

void TrendBasedAttributeGridsDialog::setGradientList( const QStringList& h){
    ui->cbGradient->clear();
    ui->cbGradient->addItems(h);
}

void TrendBasedAttributeGridsDialog::setComputeTrend(bool on){
    ui->cbComputeTrend->setChecked(on);
}

void TrendBasedAttributeGridsDialog::setTrendIntercept(double a){
    ui->leTrendIntercept->setText(QString::number(a));
}

void TrendBasedAttributeGridsDialog::setTrendAngle(double a){
    ui->leTrendAngle->setText(QString::number(a));
}

QMap<QString,QString> TrendBasedAttributeGridsDialog::params(){

    QMap<QString, QString> p;

    p.insert("attribute", QString::number(ui->cbType->currentIndex()));
    p.insert("intercept", ui->cbIntercept->currentText());
    p.insert("gradient", ui->cbGradient->currentText());
    bool computeTrend=ui->cbComputeTrend->isChecked();
    p.insert("compute-trend", QString::number(computeTrend ) );
    p.insert("trend-angle", (computeTrend) ? ui->leTrendAngle->text() : "" );
    p.insert("trend-intercept", (computeTrend) ? ui->leTrendIntercept->text() : "" );
    QString fullGridName=createFullGridName( GridType::Attribute, ui->leOutput->text() );
    p.insert("output", fullGridName );

    return p;
}


void TrendBasedAttributeGridsDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if(  reservedGrids().contains(ui->leOutput->text() ) ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
