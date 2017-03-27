#include "frequencyvolumedialog.h"
#include "ui_frequencyvolumedialog.h"

#include<QIntValidator>
#include<QPalette>
#include<QPushButton>

#include<horizonwindowposition.h>

FrequencyVolumeDialog::FrequencyVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FrequencyVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* windowValidator=new QIntValidator(this);
    ui->leWindowStart->setValidator(windowValidator);
    ui->leWindowEnd->setValidator(windowValidator);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leMinimumFrequency->setValidator(dvalidator);
    ui->leMaximumFrequency->setValidator(dvalidator);

    HorizonWindowPosition pos[]={HorizonWindowPosition::Above, HorizonWindowPosition::Center,
                                HorizonWindowPosition::Below};
    for( auto p : pos){
        ui->cbPosition->addItem(toQString(p));
    }

    connect( ui->leMinimumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaximumFrequency, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect( ui->leVolumeName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leWindowEnd, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbTimeWindow, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );

    connect( ui->cbTimeWindow, SIGNAL(toggled(bool)), ui->leWindowStart, SLOT(setEnabled(bool)) );
    connect( ui->cbTimeWindow, SIGNAL(toggled(bool)), ui->leWindowEnd, SLOT(setEnabled(bool)) );
}

FrequencyVolumeDialog::~FrequencyVolumeDialog()
{
    delete ui;
}

void FrequencyVolumeDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void FrequencyVolumeDialog::setWindowStartMS( int t ){
    ui->leWindowStart->setText(QString::number(t));
}

void FrequencyVolumeDialog::setWindowEndMS( int t ){
    ui->leWindowEnd->setText(QString::number(t));
}

QMap<QString,QString> FrequencyVolumeDialog::params(){

    QMap<QString, QString> p;


    p.insert( QString("volume"), ui->leVolumeName->text() );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    if( ui->cbTimeWindow->isChecked()){

        p.insert("window-mode", "TRUE");
        p.insert("window-start", ui->leWindowStart->text());
        p.insert("window-end", ui->leWindowEnd->text());

    }

    p.insert( QString("minimum-frequency"), ui->leMinimumFrequency->text() );

    p.insert( QString("maximum-frequency"), ui->leMaximumFrequency->text() );

    p.insert( QString("window-samples"), ui->cbWindowSamples->currentText() );

    p.insert( QString("window-position"), ui->cbPosition->currentText());

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
