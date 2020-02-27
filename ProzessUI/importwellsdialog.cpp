#include "importwellsdialog.h"
#include "ui_importwellsdialog.h"

#include <table.h>

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>
#include<gridformat.h>
#include<QProgressDialog>
#include<QStringList>


ImportWellsDialog::ImportWellsDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ImportWellsDialog)
{
    ui->setupUi(this);

    connect( ui->leFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
 }

ImportWellsDialog::~ImportWellsDialog()
{
    delete ui;
}

QMap<QString,QString> ImportWellsDialog::params(){

    QMap<QString, QString> p;

    p.insert( "file", ui->leFile->text());
    p.insert( "name-column", QString::number(ui->sbName->value()));
    p.insert( "uwi-column", QString::number(ui->sbUwi->value()));
    p.insert( "x-column", QString::number(ui->sbX->value()));
    p.insert( "y-column", QString::number(ui->sbY->value()));
    p.insert( "z-column", QString::number(ui->sbZ->value()));
    p.insert( "skip-lines", QString::number(ui->sbSkip->value()));

    return p;
}


void ImportWellsDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(nullptr, "Select table file");

    if( fn.isNull()) return;        // cancelled

    ui->leFile->setText(fn);

    updatePreview(fn);
}


void ImportWellsDialog::updatePreview(const QString& filename)
{

    ui->teContent->clear();

    QFile file( filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    int i=0;
    while(!in.atEnd() && i<PreviewLines){
        QString line=in.readLine();
        ui->teContent->append(line);
        i++;
    }



   ui->teContent -> moveCursor (QTextCursor::Start) ;
   ui->teContent -> ensureCursorVisible() ;

}


 void ImportWellsDialog::updateOkButton(){

     bool ok=true;

     if( ui->leFile->text().isEmpty()) ok=false;

     ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
 }
