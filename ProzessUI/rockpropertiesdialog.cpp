#include "rockpropertiesdialog.h"
#include "ui_rockpropertiesdialog.h"
#include<QComboBox>
#include<QPushButton>

#include<smoothprocessor.h>
#include<iostream>

RockPropertiesDialog::RockPropertiesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::RockPropertiesDialog)
{
    ui->setupUi(this);

    connect( ui->leYoungsModulus, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leBulkModulus, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leShearModulus, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lePoissonRatio, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbWell, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLogs()) );
}

RockPropertiesDialog::~RockPropertiesDialog()
{
    delete ui;
}

QMap<QString,QString> RockPropertiesDialog::params(){

    QMap<QString, QString> p;

    p.insert("well", ui->cbWell->currentText() );
    p.insert( "dt", ui->cbDT->currentText() );
    p.insert( "dts", ui->cbDTS->currentText() );
    p.insert( "den", ui->cbDEN->currentText() );
    p.insert( "youngs-modulus", ui->leYoungsModulus->text() );
    p.insert( "bulk-modulus", ui->leBulkModulus->text() );
    p.insert( "shear-modulus", ui->leShearModulus->text() );
    p.insert( "poisson-ratio", ui->lePoissonRatio->text() );
    return p;
}


void RockPropertiesDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateWells();
}


void RockPropertiesDialog::updateLogs(){

    ui->cbDT->clear();
    ui->cbDTS->clear();
    ui->cbDEN->clear();

    if( !m_project ) return;

    auto well=ui->cbWell->currentText();
    auto loglist=m_project->logList(well);
    std::sort( loglist.begin(), loglist.end());

    ui->cbDT->addItems(loglist);
    ui->cbDTS->addItems(loglist);
    ui->cbDEN->addItems(loglist);

    updateOkButton();
}


void RockPropertiesDialog::updateWells(){

    ui->cbWell->clear();

    if( !m_project) return;

    QStringList welllist=m_project->wellList();
    std::sort( welllist.begin(), welllist.end());

    ui->cbWell->addItems(welllist);
    updateLogs();
}


void RockPropertiesDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;

    auto well=ui->cbWell->currentText();
    if( well.isEmpty() ) ok=false;

    for( auto le : {ui->leYoungsModulus, ui->leShearModulus, ui->leBulkModulus, ui->lePoissonRatio} ){
        auto name=le->text();
        if( name.isEmpty() ) ok=false;

        QPalette palette=le->palette();
        if( m_project->existsLog(well, name) ){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
            break;
        }

        le->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
