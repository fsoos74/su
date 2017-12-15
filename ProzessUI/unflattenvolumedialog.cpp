#include "unflattenvolumedialog.h"
#include "ui_unflattenvolumedialog.h"

#include<avoproject.h>
#include<QIntValidator>
#include<QPushButton>


UnflattenVolumeDialog::UnflattenVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::UnflattenVolumeDialog)
{
    ui->setupUi(this);
    QIntValidator* validator=new QIntValidator(this);
    ui->leStart->setValidator(validator);
    ui->leLength->setValidator(validator);

    connect( ui->leOutputName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leStart, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leLength, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

UnflattenVolumeDialog::~UnflattenVolumeDialog()
{
    delete ui;
}

void UnflattenVolumeDialog::setInputs( const QStringList& h){
    ui->cbInputName->clear();
    ui->cbInputName->addItems(h);
}

void UnflattenVolumeDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> UnflattenVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-volume"),  ui->leOutputName->text() );

    p.insert( QString("input-volume"), ui->cbInputName->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("start"), ui->leStart->text());

    p.insert( QString("length"), ui->leLength->text());

    return p;
}

void UnflattenVolumeDialog::updateOkButton(){

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

    if( ui->leStart->text().isEmpty()){
        ok=false;
    }

    if( ui->leLength->text().isEmpty()){
        ok=false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

