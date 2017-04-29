#include "horizoncurvaturedialog.h"
#include "ui_horizoncurvaturedialog.h"
#include<QPushButton>

#include<horizoncurvatureprocess.h> // need this to get same attriute strings


HorizonCurvatureDialog::HorizonCurvatureDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::HorizonCurvatureDialog)
{
    ui->setupUi(this);

    connect( ui->leBaseName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));

    updateOkButton();
}

HorizonCurvatureDialog::~HorizonCurvatureDialog()
{
    delete ui;
}

void HorizonCurvatureDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> HorizonCurvatureDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );
    p.insert( QString("basename"), ui->leBaseName->text() );

    // attributes, maybe find better way later
    p.insert( MEAN_CURVATURE_STR, QString::number( ui->cbKMean->isChecked() ) );
    p.insert( GAUSS_CURVATURE_STR, QString::number( ui->cbKGauss->isChecked() ) );
    p.insert( MIN_CURVATURE_STR, QString::number( ui->cbKMin->isChecked() ) );
    p.insert( MAX_CURVATURE_STR, QString::number( ui->cbKMax->isChecked() ) );
    p.insert( MAX_POS_CURVATURE_STR, QString::number( ui->cbKMaxPos->isChecked() ) );
    p.insert( MAX_NEG_CURVATURE_STR, QString::number( ui->cbKMaxNeg->isChecked() ) );
    p.insert( DIP_ANGLE_STR, QString::number( ui->cbDipAngle->isChecked() ) );
    p.insert( DIP_AZIMUTH_STR, QString::number( ui->cbDipAzimuth->isChecked() ) );

    return p;
}

void HorizonCurvatureDialog::updateOkButton(){

    bool ok=true;

    if( ui->cbHorizon->currentText().isNull() ){
        ok=false;
    }

    if( ui->leBaseName->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
