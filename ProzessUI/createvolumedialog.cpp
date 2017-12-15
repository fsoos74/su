#include "createvolumedialog.h"
#include "ui_createvolumedialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QPushButton>
#include <wellpath.h>
#include<cmath>
#include <domain.h>
#include <volumetype.h>


CreateVolumeDialog::CreateVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CreateVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leMinIline->setValidator(ivalidator);
    ui->leMaxIline->setValidator(ivalidator);
    ui->leMinXline->setValidator(ivalidator);
    ui->leMaxXline->setValidator(ivalidator);
    ui->leMinZ->setValidator(ivalidator);
    ui->leMaxZ->setValidator(ivalidator);
    ui->cbDZ->setValidator(ivalidator);

    connect(ui->leMinIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->cbDZ, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    ui->cbDomain->addItem(toQString(Domain::Time));
    ui->cbDomain->addItem(toQString(Domain::Depth));
    ui->cbDomain->addItem(toQString(Domain::Other));

    ui->cbType->addItem(toQString(VolumeType::AVO));
    ui->cbType->addItem(toQString(VolumeType::VEL));
    ui->cbType->addItem(toQString(VolumeType::Other));

    updateOkButton();
}

CreateVolumeDialog::~CreateVolumeDialog()
{
    delete ui;
}


void CreateVolumeDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    if( m_project){
        auto geom=m_project->geometry();
        ui->leMinIline->setText(QString::number(geom.bboxLines().left()));
        ui->leMaxIline->setText(QString::number(geom.bboxLines().right()));
        ui->leMinXline->setText(QString::number(geom.bboxLines().top()));
        ui->leMaxXline->setText(QString::number(geom.bboxLines().bottom()));
    }
}

QMap<QString, QString> CreateVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("volume"), ui->leVolume->text());
    p.insert( "domain", ui->cbDomain->currentText());
    p.insert( "type", ui->cbType->currentText());
    p.insert( QString("min-iline"), ui->leMinIline->text());
    p.insert( QString("max-iline"), ui->leMaxIline->text());
    p.insert( QString("min-xline"), ui->leMinXline->text());
    p.insert( QString("max-xline"), ui->leMaxXline->text());
    p.insert( QString("min-z"), ui->leMinZ->text());
    p.insert( QString("max-z"), ui->leMaxZ->text());
    p.insert( QString("dz"), ui->cbDZ->currentText());

    return p;
}

void CreateVolumeDialog::updateOkButton(){

    bool ok=true;

    // test volume name
    if( ui->leVolume->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( m_project && m_project->volumeList().contains(ui->leVolume->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolume->setPalette(palette);
    }


    // test inline range
    {
    QPalette palette;
    if( ui->leMinIline->text().toInt() >= ui->leMaxIline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinIline->setPalette(palette);
    ui->leMaxIline->setPalette(palette);
    }

    // test crossline range
    {
    QPalette palette;
    if( ui->leMinXline->text().toInt() >=ui->leMaxXline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinXline->setPalette(palette);
    ui->leMaxXline->setPalette(palette);
    }

    // test depth/time range
    {
    QPalette palette;
    if( ui->leMinZ->text().toInt() >= ui->leMaxZ->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinZ->setPalette(palette);
    ui->leMaxZ->setPalette(palette);
    }

    // test sampling interval
    {
    QPalette palette;
    if( ui->cbDZ->currentText().toInt() <= 0 ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->cbDZ->setPalette(palette);
    }


    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

