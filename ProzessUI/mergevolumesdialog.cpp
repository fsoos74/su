#include "mergevolumesdialog.h"
#include "ui_mergevolumesdialog.h"

#include<avoproject.h>
#include<QPushButton>
#include<QDoubleValidator>


MergeVolumesDialog::MergeVolumesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::MergeVolumesDialog)
{
    ui->setupUi(this);

    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

MergeVolumesDialog::~MergeVolumesDialog()
{
    delete ui;
}

void MergeVolumesDialog::setInputVolumes( const QStringList& h){
    ui->cbVolume1->clear();
    ui->cbVolume1->addItems(h);
    ui->cbVolume2->clear();
    ui->cbVolume2->addItems(h);
}

QMap<QString,QString> MergeVolumesDialog::params(){

    QMap<QString, QString> p;

    p.insert("overlap", QString::number(ui->cbOverlap->currentIndex()));
    p.insert("volume1", ui->cbVolume1->currentText());
    p.insert("volume2", ui->cbVolume2->currentText());
    p.insert("output", ui->leOutput->text() );

    return p;
}


void MergeVolumesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedVolumes().contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
