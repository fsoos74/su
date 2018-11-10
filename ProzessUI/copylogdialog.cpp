#include "copylogdialog.h"
#include "ui_copylogdialog.h"
#include<QComboBox>
#include<QPushButton>


CopyLogDialog::CopyLogDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CopyLogDialog)
{
    ui->setupUi(this);

    connect( ui->cbSourceWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateLogs()) );
    connect( ui->cbSourceLog, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbDestinationWell, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leDestinationLog, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

CopyLogDialog::~CopyLogDialog()
{
    delete ui;
}

QMap<QString,QString> CopyLogDialog::params(){

    QMap<QString, QString> p;
    p.insert( "source-well", ui->cbSourceWell->currentText() );
    p.insert( "source-log", ui->cbSourceLog->currentText() );
    p.insert( "destination-well", ui->cbDestinationWell->currentText() );
    p.insert( "destination-log", ui->leDestinationLog->text() );
    p.insert( "description", ui->leDescr->text() );
    return p;
}


void CopyLogDialog::setProject(AVOProject* project){

    if( project==m_project) return;
    m_project = project;
    updateWells();
}


void CopyLogDialog::updateLogs(){

    ui->cbSourceLog->clear();

    if( !m_project ) return;

    auto llist=m_project->logList(ui->cbSourceWell->currentText());
    std::sort( llist.begin(), llist.end());

    ui->cbSourceLog->addItems(llist);
}


void CopyLogDialog::updateWells(){

    ui->cbSourceWell->clear();
    ui->cbDestinationWell->clear();

    if( !m_project) return;

    auto wlist=m_project->wellList();
    std::sort( wlist.begin(), wlist.end());
    ui->cbSourceWell->addItems(wlist);
    ui->cbDestinationWell->addItems(wlist);
}


void CopyLogDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    auto oname=ui->leDestinationLog->text();

    if( oname.isEmpty() ) ok=false;
    QPalette palette;
    if( m_project->existsLog(ui->cbDestinationWell->currentText(), oname) ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leDestinationLog->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
