#include "exportvolumerefdialog.h"
#include "ui_exportvolumerefdialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QFileDialog>
#include <avoproject.h>

ExportVolumeRefDialog::ExportVolumeRefDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportVolumeRefDialog)
{
    ui->setupUi(this);

    QDoubleValidator* dvalidator=new QDoubleValidator(this);

    ui->leNULL->setValidator(dvalidator);

    updateOkButton();
}

ExportVolumeRefDialog::~ExportVolumeRefDialog()
{
    delete ui;
}

void ExportVolumeRefDialog::setProject( AVOProject* project){
    if(project==mProject ) return;
    mProject=project;
    ui->cbVolume->clear();
    ui->cbRefStack->clear();

    if(mProject){
        ui->cbVolume->addItems(mProject->volumeList());
        ui->cbRefStack->addItems(mProject->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));
    }

    updateOkButton();
}


void ExportVolumeRefDialog::on_pbBrowse_clicked()
{
    QString filter = "SEG-Y Files (*.sgy)";

    QFileDialog dialog(this, tr("Export Volume"), QDir::homePath(), filter);
    dialog.setDefaultSuffix(tr(".sgy"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if( dialog.exec()==QDialog::Accepted && !dialog.selectedFiles().empty()){

        QString fn=dialog.selectedFiles().front();

       if( !fn.isNull()){
            ui->leOutputFile->setText(fn);
            updateOkButton();
        }
    }
}


QMap<QString,QString> ExportVolumeRefDialog::params(){

    QMap<QString, QString> p;

    p.insert( "volume", ui->cbVolume->currentText());
    p.insert( "output-file", ui->leOutputFile->text() );
    p.insert( "ref-stack", ui->cbRefStack->currentText());
    p.insert( "null-value", ui->leNULL->text());
    p.insert( "ebcdic", ui->teDescription->toPlainText());
    return p;
}


void ExportVolumeRefDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputFile->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
