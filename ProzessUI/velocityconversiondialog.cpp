#include "velocityconversiondialog.h"
#include "ui_velocityconversiondialog.h"
#include<QComboBox>
#include<QPushButton>
#include <QIntValidator>
#include <volumetype.h>

VelocityConversionDialog::VelocityConversionDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VelocityConversionDialog)
{
    ui->setupUi(this);

    auto ivalidator=new QIntValidator(this);
    ivalidator->setBottom(1);
    ui->cbSampling->setValidator(ivalidator);
    ui->leMax->setValidator(ivalidator);

    ui->cbType->addItem(toQString(VolumeType::IVEL));
    ui->cbType->addItem(toQString(VolumeType::AVEL));

    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

VelocityConversionDialog::~VelocityConversionDialog()
{
    delete ui;
}

QMap<QString,QString> VelocityConversionDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input-volume", ui->cbInput->currentText() );
    p.insert( "output-volume", ui->leOutput->text() );
    p.insert( "type", ui->cbType->currentText() );
    p.insert( "dz", ui->cbSampling->currentText() );
    p.insert( "lz", ui->leMax->text() );

    return p;
}


void VelocityConversionDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputVolumes();
}


void VelocityConversionDialog::updateInputVolumes(){

    if( !m_project) return;
    ui->cbInput->clear();
    for( auto volume : m_project->volumeList() ){
        Grid3DBounds bounds;
        Domain domain;
        VolumeType type;
        try{
            bounds=m_project->getVolumeBounds(volume, &domain, &type);
        }
        catch(...){
            continue;	// silently ignore
        }
        if(domain!=Domain::Depth) continue;	// restricted to depth for now
        if(type==VolumeType::AVEL || type==VolumeType::IVEL){
            ui->cbInput->addItem(volume);
        }
    }
}

void VelocityConversionDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutput->text().isEmpty()){
          ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leOutput->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutput->setPalette(palette);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}
