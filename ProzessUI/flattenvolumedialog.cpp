#include "flattenvolumedialog.h"
#include "ui_flattenvolumedialog.h"

#include<avoproject.h>
#include<QIntValidator>
#include<QPushButton>

#include<flattenvolumeprocess.h>    // mode

FlattenVolumeDialog::FlattenVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FlattenVolumeDialog)
{
    ui->setupUi(this);

    ui->cbMode->addItem(FlattenVolumeProcess::toQString(FlattenVolumeProcess::Mode::Flatten));
    ui->cbMode->addItem(FlattenVolumeProcess::toQString(FlattenVolumeProcess::Mode::Unflatten));

    QIntValidator* validator=new QIntValidator(this);
    ui->leStart->setValidator(validator);
    ui->leLength->setValidator(validator);

    connect( ui->leOutputName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leLength, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

FlattenVolumeDialog::~FlattenVolumeDialog()
{
    delete ui;
}

void FlattenVolumeDialog::setInputs( const QStringList& h){
    ui->cbInputName->clear();
    ui->cbInputName->addItems(h);
}

void FlattenVolumeDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> FlattenVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( "output-volume",  ui->leOutputName->text() );
    p.insert( "input-volume", ui->cbInputName->currentText());
    p.insert( "horizon", ui->cbHorizon->currentText() );
    p.insert( "start", ui->leStart->text());
    p.insert( "length", ui->leLength->text());
    p.insert( "mode", ui->cbMode->currentText());
    return p;
}

void FlattenVolumeDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputName->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedVolumes().contains(ui->leOutputName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputName->setPalette(palette);

    if( ui->leLength->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

