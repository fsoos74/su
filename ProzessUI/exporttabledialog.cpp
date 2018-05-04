#include "exporttabledialog.h"
#include "ui_exporttabledialog.h"

#include<QFileDialog>
#include<QStandardPaths>
#include<gridformat.h>


ExportTableDialog::ExportTableDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportTableDialog)
{
    ui->setupUi(this);

    ui->cbFormat->addItem(toQString(GridFormat::XYZ));
    ui->cbFormat->addItem(toQString(GridFormat::ILXLZ));
    ui->cbFormat->addItem(toQString(GridFormat::XYILXLZ));


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
    ui->cbTable->setEnabled(true);
    updateOkButton();
}

void ExportTableDialog::setFixedTable(const QString & name){
    ui->cbTable->clear();
    ui->cbTable->addItem(name);
    ui->cbTable->setEnabled(false);
}

void ExportTableDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getSaveFileName(nullptr, tr("export Table"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if( fn.isEmpty()) return;
    ui->leOutputFile->setText(fn);
}


QMap<QString,QString> ExportTableDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("table"), ui->cbTable->currentText());
    p.insert( "format", ui->cbFormat->currentText());
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
