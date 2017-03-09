#include "curvaturevolumedialog.h"
#include "ui_curvaturevolumedialog.h"

#include<QPushButton>

CurvatureVolumeDialog::CurvatureVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CurvatureVolumeDialog)
{
    ui->setupUi(this);

    connect( ui->cbInputVolume, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

CurvatureVolumeDialog::~CurvatureVolumeDialog()
{
    delete ui;
}


void CurvatureVolumeDialog::setInputVolumes( const QStringList& l){

    m_inputVolumes=l;

    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(m_inputVolumes);

    updateOkButton();
}



QMap<QString,QString> CurvatureVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText() );
    p.insert( QString("output-volume"), ui->leOutputVolume->text() );

    return p;
}


void CurvatureVolumeDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputVolume->text().isEmpty()){
        ok=false;
    }

    // output grid
    {
    QPalette palette;
    if( m_inputVolumes.contains(ui->leOutputVolume->text() ) ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputVolume->setPalette(palette);

    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

