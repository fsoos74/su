#include "semblancevolumedialog.h"
#include "ui_semblancevolumedialog.h"

#include<QPushButton>
#include<avoproject.h>

SemblanceVolumeDialog::SemblanceVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::SemblanceVolumeDialog)
{
    ui->setupUi(this);

    connect( ui->leVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

SemblanceVolumeDialog::~SemblanceVolumeDialog()
{
    delete ui;
}

void SemblanceVolumeDialog::setInputVolumes( const QStringList& h){
    ui->cbInputVolume->clear();
    ui->cbInputVolume->addItems(h);
}

QMap<QString,QString> SemblanceVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-volume"), ui->leVolume->text() );

    p.insert( QString("input-volume"), ui->cbInputVolume->currentText());

    p.insert( QString("half-ilines"), QString::number(ui->sbInlines->value()/2) );
    p.insert( QString("half-xlines"), QString::number(ui->sbCrosslines->value()/2) );
    p.insert( QString("half-samples"), QString::number(ui->sbSamples->value()/2) );


    return p;
}

void SemblanceVolumeDialog::updateOkButton(){

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
