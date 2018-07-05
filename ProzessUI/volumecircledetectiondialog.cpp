#include "volumecircledetectiondialog.h"
#include "ui_volumecircledetectiondialog.h"

#include<avoproject.h>
#include<QPushButton>
#include<QIntValidator>

VolumeCircleDetectionDialog::VolumeCircleDetectionDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeCircleDetectionDialog)
{
    ui->setupUi(this);
    auto validator=new QIntValidator(this);
    validator->setBottom( 1);
    ui->leMinRadius->setValidator(validator);
    ui->leMaxRadius->setValidator(validator);
    connect( ui->leOutputName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

VolumeCircleDetectionDialog::~VolumeCircleDetectionDialog()
{
    delete ui;
}

void VolumeCircleDetectionDialog::setInputs( const QStringList& h){
    ui->cbInputName->clear();
    ui->cbInputName->addItems(h);
}


QMap<QString,QString> VolumeCircleDetectionDialog::params(){

    QMap<QString, QString> p;
    p.insert( "output-volume",  ui->leOutputName->text() );
    p.insert( "input-volume", ui->cbInputName->currentText());
    p.insert( "min-radius", ui->leMinRadius->text() );
    p.insert( "max-radius", ui->leMaxRadius->text() );
    return p;
}

void VolumeCircleDetectionDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputName->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedVolumes().contains(ui->leOutputName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputName->setPalette(palette);
    }

    {
    QPalette palette;
    if( ui->leMinRadius->text().toInt()>ui->leMaxRadius->text().toInt()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinRadius->setPalette(palette);
    ui->leMaxRadius->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

