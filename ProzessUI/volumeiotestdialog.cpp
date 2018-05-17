#include "volumeiotestdialog.h"
#include "ui_volumeiotestdialog.h"

#include<avoproject.h>
#include<QPushButton>


VolumeIOTestDialog::VolumeIOTestDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeIOTestDialog)
{
    ui->setupUi(this);

    connect( ui->leOutputName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

VolumeIOTestDialog::~VolumeIOTestDialog()
{
    delete ui;
}

void VolumeIOTestDialog::setInputs( const QStringList& h){
    ui->cbInputName->clear();
    ui->cbInputName->addItems(h);
}


QMap<QString,QString> VolumeIOTestDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-volume"),  ui->leOutputName->text() );

    p.insert( QString("input-volume"), ui->cbInputName->currentText());

    return p;
}

void VolumeIOTestDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputName->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leOutputName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputName->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

