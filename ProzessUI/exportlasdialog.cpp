#include "exportlasdialog.h"
#include "ui_exportlasdialog.h"

#include<QComboBox>
#include<QPushButton>
#include<QFileDialog>
#include<QStandardPaths>
#include<vshaleprocess.h>


ExportLASDialog::ExportLASDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportLASDialog)
{
    ui->setupUi(this);

    connect( ui->cbWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInputLogs(QString)) );
    connect( ui->leFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwLogs->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()));
    updateOkButton();
}

ExportLASDialog::~ExportLASDialog()
{
    delete ui;
}


QMap<QString,QString> ExportLASDialog::params(){

    QMap<QString, QString> p;

    p.insert("output-file", ui->leFile->text());
    p.insert( "well", ui->cbWell->currentText() );
    auto logs=selectedLogs();
    p.insert("logs", packParamList(logs));

    return p;
}


void ExportLASDialog::setProject(AVOProject* project){

    if( project==m_project) return;
    m_project = project;

    ui->cbWell->clear();
    auto wells=m_project->wellList();
    std::sort(wells.begin(), wells.end());
    ui->cbWell->addItems(wells);
}


void ExportLASDialog::updateInputLogs(QString well){

    ui->lwLogs->clear();
    auto logs=m_project->logList(well);
    std::sort(logs.begin(),logs.end());
    ui->lwLogs->addItems(logs);
}


void ExportLASDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    QPalette palette;
    if(ui->leFile->text().isNull() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leFile->setPalette(palette);

    if(selectedLogs().isEmpty()) ok=false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


QStringList ExportLASDialog::selectedLogs(){

    QStringList items;
    auto ids = ui->lwLogs->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        items<<ui->lwLogs->item(idx.row())->text();
    }
    return items;
}

void ExportLASDialog::on_pbSelect_clicked()
{
   static QString dir=QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).front();
   QString selfilter = tr("LAS (*.las *.LAS)");
   auto fname=QFileDialog::getSaveFileName(this, "Export LAS", dir, tr("LAS (*.las *.LAS);; All files (*.*)"), &selfilter);
   if(fname.isNull()) return;
   ui->leFile->setText(fname);
}
