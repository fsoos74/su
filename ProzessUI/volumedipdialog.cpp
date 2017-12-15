#include "volumedipdialog.h"
#include "ui_volumedipdialog.h"
#include<QComboBox>
#include<QPushButton>

#include <volumedipprocess.h>


VolumeDipDialog::VolumeDipDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeDipDialog)
{
    ui->setupUi(this);

    ui->cbMethod->addItems(VolumeDipProcess::methodList());

    connect( ui->leIlineDip, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leXlineDip, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

VolumeDipDialog::~VolumeDipDialog()
{
    delete ui;
}

QMap<QString,QString> VolumeDipDialog::params(){

    QMap<QString, QString> p;

    p.insert( "method", ui->cbMethod->currentText());
    p.insert( "input-volume", ui->cbInput->currentText() );
    p.insert( "iline-dip", ui->leIlineDip->text() );
    p.insert( "xline-dip", ui->leXlineDip->text() );
    p.insert( "window-samples", QString::number(ui->sbWindowSamples->value()));
    return p;
}


void VolumeDipDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputVolumes();
}


void VolumeDipDialog::updateInputVolumes(){

    if( !m_project) return;
    ui->cbInput->clear();
    ui->cbInput->addItems(m_project->volumeList());
}


void VolumeDipDialog::updateOkButton(){

    bool ok=true;

    for( QLineEdit* le : {ui->leIlineDip, ui->leXlineDip})
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

