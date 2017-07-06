#include "frequencyvolumedialog.h"
#include "ui_frequencyvolumedialog.h"

#include<QIntValidator>
#include<QPalette>
#include<QPushButton>


FrequencyVolumeDialog::FrequencyVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FrequencyVolumeDialog)
{
    ui->setupUi(this);


    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leMinimumFrequency->setValidator(dvalidator);
    ui->leMaximumFrequency->setValidator(dvalidator);


    connect( ui->leMinimumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaximumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect( ui->leVolumeName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

FrequencyVolumeDialog::~FrequencyVolumeDialog()
{
    delete ui;
}

void FrequencyVolumeDialog::setInputs( const QStringList& h){
    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(h);
}


QMap<QString,QString> FrequencyVolumeDialog::params(){

    QMap<QString, QString> p;


    p.insert( QString("output-volume"), ui->leVolumeName->text() );

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText());

    p.insert( QString("minimum-frequency"), ui->leMinimumFrequency->text() );

    p.insert( QString("maximum-frequency"), ui->leMaximumFrequency->text() );

    p.insert( QString("window-samples"), ui->cbWindowSamples->currentText() );

    return p;
}


void FrequencyVolumeDialog::updateOkButton(){

    bool ok=true;

    QPalette volumePalette;
    if( ui->leVolumeName->text().isEmpty() ||
            reservedVolumes().contains(ui->leVolumeName->text() ) ){
        ok=false;
        volumePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolumeName->setPalette(volumePalette);

    {
    QPalette palette;
    if( ui->leMaximumFrequency->text().toDouble() < ui->leMinimumFrequency->text().toDouble()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinimumFrequency->setPalette(palette);
    ui->leMaximumFrequency->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
