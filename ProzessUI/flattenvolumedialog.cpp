#include "flattenvolumedialog.h"
#include "ui_flattenvolumedialog.h"

#include<avoproject.h>
#include<QIntValidator>
#include<QPushButton>


FlattenVolumeDialog::FlattenVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::FlattenVolumeDialog)
{
    ui->setupUi(this);
    QIntValidator* validator=new QIntValidator(this);
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

    p.insert( QString("output-volume"),  ui->leOutputName->text() );

    p.insert( QString("input-volume"), ui->cbInputName->currentText());

    p.insert( QString("horizon"), ui->cbHorizon->currentText() );

    p.insert( QString("length"), ui->leLength->text());

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

