#include "dipscandialog.h"
#include "ui_dipscandialog.h"
#include<QComboBox>
#include<QPushButton>
#include <QDoubleValidator>


DipScanDialog::DipScanDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::DipScanDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinDip->setValidator(validator);
    ui->leMaxDip->setValidator(validator);

    connect( ui->leIlineDip, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leXlineDip, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leSemblance, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

DipScanDialog::~DipScanDialog()
{
    delete ui;
}

QMap<QString,QString> DipScanDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input", ui->cbInput->currentText() );
    p.insert( "iline-dip", ui->leIlineDip->text() );
    p.insert( "xline-dip", ui->leXlineDip->text() );
    p.insert( "semblance", ui->leSemblance->text() );
    p.insert( "window-lines", QString::number(ui->sbWindowLines->value()));
    p.insert( "window-samples", QString::number(ui->sbWindowSamples->value()));
    p.insert( "minimum-dip", ui->leMinDip->text() );
    p.insert( "maximum-dip", ui->leMaxDip->text() );
    p.insert( "dip-count", QString::number(ui->sbNumDips->value()));

    return p;
}


void DipScanDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateOkButton();
}


void DipScanDialog::setInputs(QStringList l){
    ui->cbInput->clear();
    ui->cbInput->addItems(l);
}

void DipScanDialog::updateOkButton(){

    bool ok=true;

    for( QLineEdit* le : {ui->leIlineDip, ui->leXlineDip, ui->leSemblance})
    {
        QPalette palette;
        if( m_project->existsVolume(le->text())){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
        }
        le->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

