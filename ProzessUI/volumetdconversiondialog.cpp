#include "volumetdconversiondialog.h"
#include "ui_volumetdconversiondialog.h"
#include<QComboBox>
#include<QPushButton>
#include <volumetdconversionprocess.h>	// tddirection
#include <QIntValidator>

VolumeTDConversionDialog::VolumeTDConversionDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VolumeTDConversionDialog)
{
    ui->setupUi(this);

    auto ivalidator=new QIntValidator(this);
    ivalidator->setBottom(1);
    ui->cbSampling->setValidator(ivalidator);
    ui->leMax->setValidator(ivalidator);

    ui->cbDirection->addItem(toQString(TDDirection::TIME_TO_DEPTH));
    ui->cbDirection->addItem(toQString(TDDirection::DEPTH_TO_TIME));

    connect( ui->cbDirection, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateInputVolumes()));
    connect( ui->leOutput, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

VolumeTDConversionDialog::~VolumeTDConversionDialog()
{
    delete ui;
}

QMap<QString,QString> VolumeTDConversionDialog::params(){

    QMap<QString, QString> p;

    p.insert( "input-volume", ui->cbInput->currentText() );
    p.insert( "velocity-volume", ui->cbVelocity->currentText() );
    p.insert( "output-volume", ui->leOutput->text() );
    p.insert( "direction", ui->cbDirection->currentText() );
    p.insert( "dz", ui->cbSampling->currentText() );
    p.insert( "lz", ui->leMax->text() );

    return p;
}


void VolumeTDConversionDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    updateInputVolumes();
}


void VolumeTDConversionDialog::updateInputVolumes(){

    if( !m_project) return;
    ui->cbInput->clear();
    ui->cbVelocity->clear();
    Domain idomain=(toTDDirection(ui->cbDirection->currentText()) ==
                    TDDirection::TIME_TO_DEPTH) ? Domain::Time : Domain::Depth;

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
        if(domain==idomain){
            ui->cbInput->addItem(volume);
        }
        if(domain==Domain::Depth && type==VolumeType::IVEL){
            ui->cbVelocity->addItem(volume);
        }
    }
}

void VolumeTDConversionDialog::updateOkButton(){

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
