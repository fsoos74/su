#include "exportlogdialog.h"
#include "ui_exportlogdialog.h"

#include<QComboBox>
#include<QPushButton>
#include<QDoubleValidator>
#include<QFileDialog>

ExportLogDialog::ExportLogDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ExportLogDialog)
{
    ui->setupUi(this);

    auto validator=new QDoubleValidator(this);
    ui->leMinDepth->setValidator(validator);
    ui->leMaxDepth->setValidator(validator);

    connect( ui->cbWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInputLogs(QString)) );
    connect( ui->lwLogs->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );
    connect( ui->leNull, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));
    connect( ui->cbRestrictDepths, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()));
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));
    connect( ui->leMinDepth, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));
    connect( ui->leMaxDepth, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()));

    updateOkButton();
}

ExportLogDialog::~ExportLogDialog()
{
    delete ui;
}


QMap<QString,QString> ExportLogDialog::params(){

    QMap<QString, QString> p;

    p.insert( "well", ui->cbWell->currentText() );
    //p.insert( "log", ui->cbLog->currentText() );
    auto logs=selectedLogs();
    p.insert("logs", packParamList(logs));
    p.insert( "output-file", ui->leOutput->text() );
    p.insert( "null", ui->leNull->text());
    bool restrict=ui->cbRestrictDepths->isChecked();
    p.insert( "restrict-depths", QString::number(static_cast<int>(restrict)));
    p.insert( "min-depth", ui->leMinDepth->text());
    p.insert( "max-depth", ui->leMaxDepth->text());

    return p;
}


void ExportLogDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->cbWell->clear();
    ui->cbWell->addItems(wells);
}


void ExportLogDialog::updateInputLogs(QString well){

    ui->lwLogs->clear();
    if( !m_project) return;
    ui->lwLogs->addItems(m_project->logList(well));
}

void ExportLogDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    if(selectedLogs().size()<1){
        ok=false;
    }

    if(ui->leNull->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( ui->cbRestrictDepths->isChecked() && ui->leMinDepth->text().toDouble()>ui->leMaxDepth->text().toDouble()){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinDepth->setPalette(palette);
    ui->leMaxDepth->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void ExportLogDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getSaveFileName(this, "Export Log", QDir::homePath() );
    if( fn.isEmpty() ) return;
    ui->leOutput->setText(fn);
}

QStringList ExportLogDialog::selectedLogs(){

    QStringList logs;
    auto ids = ui->lwLogs->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        logs<<ui->lwLogs->item(idx.row())->text();
    }
    return logs;
}
