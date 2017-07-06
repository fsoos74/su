#include "curvaturedialog.h"
#include "ui_curvaturedialog.h"
#include<QPushButton>

#include<horizoncurvatureprocess.h> // need this to get same attriute strings


CurvatureDialog::CurvatureDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CurvatureDialog)
{
    ui->setupUi(this);

    connect( ui->leBaseName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));

    updateOkButton();
}

CurvatureDialog::~CurvatureDialog()
{
    delete ui;
}

void CurvatureDialog::setMode( CurvatureDialog::Mode m){

    if( m==m_mode ) return;

    m_mode=m;

    ui->lbInput->setText( (m_mode==Mode::Volume) ? "Volume" : "Horizon" );
}

void CurvatureDialog::setInputs( const QStringList& h){
    ui->cbInput->clear();
    ui->cbInput->addItems(h);
}


QMap<QString,QString> CurvatureDialog::params(){

    QMap<QString, QString> p;

    p.insert( (m_mode==Mode::Horizon) ? QString("horizon") : QString("volume"), ui->cbInput->currentText() );
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

void CurvatureDialog::updateOkButton(){

    bool ok=true;

    if( ui->cbInput->currentText().isNull() ){
        ok=false;
    }

    if( ui->leBaseName->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
