#include "curvaturevolumedialog.h"
#include "ui_curvaturevolumedialog.h"
#include<QPushButton>

#include<curvature_attributes.h>


CurvatureVolumeDialog::CurvatureVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CurvatureVolumeDialog)
{
    ui->setupUi(this);

    connect( ui->leBaseName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));

    updateOkButton();
}

CurvatureVolumeDialog::~CurvatureVolumeDialog()
{
    delete ui;
}

void CurvatureVolumeDialog::setInputs( const QStringList& h){
    ui->cbInput->clear();
    ui->cbInput->addItems(h);
}


QMap<QString,QString> CurvatureVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("volume"), ui->cbInput->currentText() );
    p.insert( QString("basename"), ui->leBaseName->text() );
    p.insert( QString("window-length"), QString::number(ui->sbWindowLen->value()) );
    p.insert( QString("maximum-shift"), QString::number(ui->sbMaximumShift->value()) );
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

void CurvatureVolumeDialog::updateOkButton(){

    bool ok=true;

    if( ui->cbInput->currentText().isNull() ){
        ok=false;
    }

    if( ui->leBaseName->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
