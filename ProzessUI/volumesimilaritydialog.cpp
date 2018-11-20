#include "volumesimilaritydialog.h"
#include "ui_volumesimilaritydialog.h"
#include<QComboBox>
#include<QPushButton>



VolumeSimilarityDialog::VolumeSimilarityDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeSimilarityDialog)
{
    ui->setupUi(this);

    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbInput, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbILDip, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbXLDip, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
}

VolumeSimilarityDialog::~VolumeSimilarityDialog()
{
    delete ui;
}

QMap<QString,QString> VolumeSimilarityDialog::params(){

    QMap<QString, QString> p;
    p.insert( "output-volume", ui->leOutput->text());
    p.insert( "input-volume", ui->cbInput->currentText() );
    p.insert( "iline-dip", ui->cbILDip->currentText() );
    p.insert( "xline-dip", ui->cbXLDip->currentText() );
    return p;
}


void VolumeSimilarityDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputVolumes();
}


void VolumeSimilarityDialog::updateInputVolumes(){

    if( !m_project) return;
    ui->cbInput->clear();
    ui->cbInput->addItems(m_project->volumeList());
    ui->cbILDip->clear();
    ui->cbILDip->addItems(m_project->volumeList());
    ui->cbXLDip->clear();
    ui->cbXLDip->addItems(m_project->volumeList());
}


void VolumeSimilarityDialog::updateOkButton(){

    bool ok=true;

    QString output=ui->leOutput->text();
    QPalette palette;
    if( output.isEmpty() || m_project->existsVolume(output)){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    if(ui->cbILDip->currentText().isEmpty()) ok=false;
    if(ui->cbXLDip->currentText().isEmpty()) ok=false;
    if(ui->cbInput->currentText().isEmpty()) ok=false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

