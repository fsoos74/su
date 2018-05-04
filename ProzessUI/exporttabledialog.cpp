#include "exporttabledialog.h"
#include "ui_exporttabledialog.h"

#include<QFileDialog>

ExportTableDialog::ExportTableDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportTableDialog)
{
    ui->setupUi(this);

    connect(ui->leOutputFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

ExportTableDialog::~ExportTableDialog()
{
    delete ui;
}

void ExportTableDialog::setTables( const QStringList& h){
    ui->cbTable->clear();
    ui->cbTable->addItems(h);
    updateOkButton();
}

void ExportTableDialog::on_pbBrowse_clicked()
{
    QString filter = "SEG-Y Files (*.sgy)";

    QFileDialog dialog(this, tr("Export Table"), QDir::homePath(), filter);
    dialog.setDefaultSuffix(tr(".xyz"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if( dialog.exec()==QDialog::Accepted && !dialog.selectedFiles().empty()){

        QString fn=dialog.selectedFiles().front();

       if( !fn.isNull()){
            ui->leOutputFile->setText(fn);
        }
    }
}


QMap<QString,QString> ExportTableDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("table"), ui->cbVolume->currentText());

    p.insert( QString("output-file"), ui->leOutputFile->text() );

    return p;
}


void ExportTableDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputFile->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
