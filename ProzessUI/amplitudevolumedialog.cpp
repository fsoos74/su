#include "amplitudevolumedialog.h"
#include "ui_amplitudevolumedialog.h"

#include<QIntValidator>
#include<QPalette>
#include<QPushButton>


AmplitudeVolumeDialog::AmplitudeVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::AmplitudeVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* windowValidator=new QIntValidator(this);
    ui->leMinIline->setValidator(windowValidator);
    ui->leMaxIline->setValidator(windowValidator);
    ui->leMinXline->setValidator(windowValidator);
    ui->leMaxXline->setValidator(windowValidator);
    ui->leMinZ->setValidator(windowValidator);
    ui->leMaxZ->setValidator(windowValidator);

    connect( ui->leVolumeName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbCrop, SIGNAL( toggled(bool)), this, SLOT(updateOkButton()) );
}

AmplitudeVolumeDialog::~AmplitudeVolumeDialog()
{
    delete ui;
}


QMap<QString,QString> AmplitudeVolumeDialog::params(){

    QMap<QString, QString> p;


    p.insert( "volume", ui->leVolumeName->text() );

    p.insert( "dataset", ui->cbDataset->currentText());

    p.insert( "crop", QString::number(static_cast<int>(ui->cbCrop->isChecked())));

    p.insert( "min-iline", QString::number(minIline()));
    p.insert( "max-iline", QString::number(maxIline()));
    p.insert( "min-xline", QString::number(minXline()));
    p.insert( "max-xline", QString::number(maxXline()));
    p.insert( "min-z", QString::number(minZ()));
    p.insert( "max-z", QString::number(maxZ()));

    return p;
}



int AmplitudeVolumeDialog::minIline(){
    return ui->leMinIline->text().toInt();
}

int AmplitudeVolumeDialog::maxIline(){
    return ui->leMaxIline->text().toInt();
}

int AmplitudeVolumeDialog::minXline(){
    return ui->leMinXline->text().toInt();
}

int AmplitudeVolumeDialog::maxXline(){
    return ui->leMaxXline->text().toInt();
}

int AmplitudeVolumeDialog::minZ(){
    return ui->leMinZ->text().toInt();
}

int AmplitudeVolumeDialog::maxZ(){
    return ui->leMaxZ->text().toInt();
}


void AmplitudeVolumeDialog::setProject(AVOProject *p){
    if( p==m_project) return;
    m_project=p;
    updateDatasets();
    updateOkButton();
}


void AmplitudeVolumeDialog::setMinIline( int i ){
    ui->leMinIline->setText(QString::number(i));
}

void AmplitudeVolumeDialog::setMaxIline( int i ){
    ui->leMaxIline->setText(QString::number(i));
}

void AmplitudeVolumeDialog::setMinXline( int i ){
    ui->leMinXline->setText(QString::number(i));
}

void AmplitudeVolumeDialog::setMaxXline( int i ){
    ui->leMaxXline->setText(QString::number(i));
}


void AmplitudeVolumeDialog::setMinZ( int i ){
    ui->leMinZ->setText(QString::number(i));
}

void AmplitudeVolumeDialog::setMaxZ( int i ){
    ui->leMaxZ->setText(QString::number(i));
}



void AmplitudeVolumeDialog::updateOkButton(){

    bool ok=true;

    QPalette volumePalette;
    if( ui->leVolumeName->text().isEmpty() ||
            (m_project && m_project->volumeList().contains(ui->leVolumeName->text() ) ) ){
        ok=false;
        volumePalette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolumeName->setPalette(volumePalette);

    {
        QPalette palette;
        if( minIline()>maxIline()){
            palette.setColor(QPalette::Text, Qt::red);
            ok=false;
        }
        ui->leMinIline->setPalette(palette);
        ui->leMaxIline->setPalette(palette);
    }

    {
        QPalette palette;
        if( minXline()>maxXline()){
            palette.setColor(QPalette::Text, Qt::red);
            ok=false;
        }
        ui->leMinXline->setPalette(palette);
        ui->leMaxXline->setPalette(palette);
    }

    {
        QPalette palette;
        if( minZ()>maxZ()){
            palette.setColor(QPalette::Text, Qt::red);
            ok=false;
        }
        ui->leMinZ->setPalette(palette);
        ui->leMaxZ->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


void AmplitudeVolumeDialog::updateDatasets(){

    ui->cbDataset->clear();
    if(!m_project) return;
    ui->cbDataset->addItems(m_project->seismicDatasetList());
}
