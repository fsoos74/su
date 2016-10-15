#include "amplitudevolumedialog.h"
#include "ui_amplitudevolumedialog.h"

#include<QIntValidator>
#include<QPalette>
#include<QPushButton>


AmplitudeVolumeDialog::AmplitudeVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::AmplitudeVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* windowValidator=new QIntValidator(this);
    ui->leWindowStart->setValidator(windowValidator);
    ui->leWindowEnd->setValidator(windowValidator);

    connect( ui->leVolumeName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowEnd, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbTimeWindow, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );

    connect( ui->cbTimeWindow, SIGNAL(toggled(bool)), ui->leWindowStart, SLOT(setEnabled(bool)) );
    connect( ui->cbTimeWindow, SIGNAL(toggled(bool)), ui->leWindowEnd, SLOT(setEnabled(bool)) );
}

AmplitudeVolumeDialog::~AmplitudeVolumeDialog()
{
    delete ui;
}

void AmplitudeVolumeDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void AmplitudeVolumeDialog::setWindowStartMS( int t ){
    ui->leWindowStart->setText(QString::number(t));
}

void AmplitudeVolumeDialog::setWindowEndMS( int t ){
    ui->leWindowEnd->setText(QString::number(t));
}

QMap<QString,QString> AmplitudeVolumeDialog::params(){

    QMap<QString, QString> p;


    p.insert( QString("volume"), ui->leVolumeName->text() );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    if( ui->cbTimeWindow->isChecked()){

        p.insert("window-mode", "TRUE");
        p.insert("window-start", ui->leWindowStart->text());
        p.insert("window-end", ui->leWindowEnd->text());

    }

    return p;
}


void AmplitudeVolumeDialog::updateOkButton(){

    bool ok=true;

    QPalette volumePalette;
    if( ui->leVolumeName->text().isEmpty() ||
            reservedVolumes().contains(ui->leVolumeName->text() ) ){
        ok=false;
        volumePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolumeName->setPalette(volumePalette);

    if( ui->cbTimeWindow->isChecked() ){
        if( ui->leWindowStart->text().isEmpty()){
            ok=false;
        }
        if( ui->leWindowEnd->text().isEmpty()){
            ok=false;
        }

        QPalette windowPalette;
        if( ui->leWindowStart->text().toInt()>ui->leWindowEnd->text().toInt()){
            windowPalette.setColor(QPalette::Text, Qt::red);
        }
        ui->leWindowStart->setPalette(windowPalette);
        ui->leWindowEnd->setPalette(windowPalette);

    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
