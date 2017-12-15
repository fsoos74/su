#include "exportgriddialog.h"
#include "ui_exportgriddialog.h"


#include <QFileDialog>
#include <QStandardPaths>
#include <QPushButton>

#include<avoproject.h>
#include<gridformat.h>


ExportGridDialog::ExportGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportGridDialog)
{
    ui->setupUi(this);

    connect( ui->cbType, SIGNAL(currentIndexChanged(QString)), this, SLOT(setType(QString)) );

    connect( ui->leFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leNULL, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    ui->cbType->addItem(toQString(GridType::Horizon));
    ui->cbType->addItem(toQString(GridType::Attribute));
    ui->cbType->addItem(toQString(GridType::Other));

    ui->cbFormat->addItem(toQString(GridFormat::XYZ));
    ui->cbFormat->addItem(toQString(GridFormat::ILXLZ));
    ui->cbFormat->addItem(toQString(GridFormat::XYILXLZ));

    updateOkButton();
}

ExportGridDialog::~ExportGridDialog()
{
    delete ui;
}

QMap<QString,QString> ExportGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input-type", ui->cbType->currentText());
    p.insert( "input-name", ui->cbName->currentText());
    p.insert( "format", ui->cbFormat->currentText());
    p.insert( "null-value", ui->leNULL->text() );
    p.insert( "output-file", ui->leFile->text() );

    return p;
}

void ExportGridDialog::setProject(AVOProject* p){
    m_project=p;
    auto t = toGridType( ui->cbType->currentText());
    setType(t);
}

void ExportGridDialog::on_pbSelect_clicked()
{
    QString fn=QFileDialog::getSaveFileName(nullptr, tr("export Grid"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if( fn.isEmpty()) return;
    ui->leFile->setText(fn);
}


void ExportGridDialog::setType(QString s){
    setType(toGridType(s));
}

void ExportGridDialog::setType(GridType t){
    if(!m_project) return;
    ui->cbName->clear();
    ui->cbName->addItems(m_project->gridList(t));
}


void ExportGridDialog::setName(QString name){
    ui->cbName->setCurrentText(name);
}

void ExportGridDialog::updateOkButton(){

    bool ok=true;

    if( ui->leFile->text().isEmpty()) ok=false;

    if( ui->leNULL->text().isEmpty()) ok=false;

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}
