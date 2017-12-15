#include "volumestatisticsdialog.h"
#include "ui_volumestatisticsdialog.h"

#include<QPushButton>
#include<avoproject.h>

VolumeStatisticsDialog::VolumeStatisticsDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeStatisticsDialog)
{
    ui->setupUi(this);

    connect( ui->leVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

VolumeStatisticsDialog::~VolumeStatisticsDialog()
{
    delete ui;
}

void VolumeStatisticsDialog::setInputVolumes( const QStringList& h){
    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(h);
    ui->cbIldip->clear();
    ui->cbIldip->addItems(h);
    ui->cbXldip->clear();
    ui->cbXldip->addItems(h);
}

QMap<QString,QString> VolumeStatisticsDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-volume"), ui->leVolume->text() );

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText());

    p.insert( QString("statistic"), ui->cbType->currentText() );
    p.insert( QString("half-ilines"), QString::number(ui->sbInlines->value()/2) );
    p.insert( QString("half-xlines"), QString::number(ui->sbCrosslines->value()/2) );
    p.insert( QString("half-samples"), QString::number(ui->sbSamples->value()/2) );

    p.insert( "track-dips", QString::number( static_cast<int>(ui->cbTrackDips->isChecked() ) ) );
    p.insert( "iline-dip", ui->cbIldip->currentText());
    p.insert( "xline-dip", ui->cbXldip->currentText());

    return p;
}

void VolumeStatisticsDialog::updateOkButton(){

    bool ok=true;

    if( ui->leVolume->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leVolume->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolume->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
