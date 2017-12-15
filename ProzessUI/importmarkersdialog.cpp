#include "importmarkersdialog.h"
#include "ui_importmarkersdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>
#include<QProgressDialog>
#include<QStringList>


ImportMarkersDialog::ImportMarkersDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ImportMarkersDialog)
{
    ui->setupUi(this);

    connect( ui->leFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

ImportMarkersDialog::~ImportMarkersDialog()
{
    delete ui;
}

QMap<QString,QString> ImportMarkersDialog::params(){

    QMap<QString, QString> p;

    p.insert( "file", ui->leFile->text());
    p.insert( "separator", ui->cbSeparator->currentText());
    p.insert( "uwi-column", QString::number(ui->sbUWI->value()));
    p.insert( "name-column", QString::number(ui->sbName->value()));
    p.insert( "md-column", QString::number(ui->sbMD->value()));
    p.insert( "skip-lines", QString::number(ui->sbSkip->value()));
    p.insert( "replace", QString::number(static_cast<int>(ui->cbReplace->isChecked())));

    return p;
}


void ImportMarkersDialog::setSeparator(QString s){
    ui->cbSeparator->setCurrentText(s);
}

void ImportMarkersDialog::setUWIColumn( int col ){

    ui->sbUWI->setValue(col);
}

void ImportMarkersDialog::setNameColumn( int col ){

    ui->sbName->setValue(col);
}

void ImportMarkersDialog::setMDColumn( int col ){

    ui->sbMD->setValue(col);
}


void ImportMarkersDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Select Markers file");

    if( fn.isNull()) return;        // cancelled

    ui->leFile->setText(fn);

    updatePreview(fn);
}


void ImportMarkersDialog::updatePreview(const QString& filename)
{

    ui->teContent->clear();

    QFile file( filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Import Markers"),
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


 void ImportMarkersDialog::updateOkButton(){

     bool ok=true;

     if( ui->leFile->text().isEmpty()) ok=false;

     ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
 }
