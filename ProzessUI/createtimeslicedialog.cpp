#include "createtimeslicedialog.h"
#include "ui_createtimeslicedialog.h"

#include<avoproject.h>
#include<QIntValidator>
#include<QPushButton>


CreateTimesliceDialog::CreateTimesliceDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::CreateTimesliceDialog)
{
    ui->setupUi(this);
    QIntValidator* validator=new QIntValidator(this);
    ui->leTime->setValidator(validator);

    connect( ui->leSliceName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leTime, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

CreateTimesliceDialog::~CreateTimesliceDialog()
{
    delete ui;
}

void CreateTimesliceDialog::setDatasets( const QStringList& h){
    ui->cbDataset->clear();
    ui->cbDataset->addItems(h);
}

void CreateTimesliceDialog::setHorizons( const QStringList& h){
    ui->cbHorizon->clear();
    ui->cbHorizon->addItems(h);
}


QMap<QString,QString> CreateTimesliceDialog::params(){

    QMap<QString, QString> p;

    QString fullSliceName=createFullGridName( GridType::Other, ui->leSliceName->text());

    p.insert( QString("slice"), fullSliceName );

    p.insert( QString("dataset"), ui->cbDataset->currentText());

    if( ui->cbUseHorizon->isChecked()){
        p.insert( QString("horizon"), ui->cbHorizon->currentText() );
    }
    else{
        p.insert( QString("time"), ui->leTime->text());
    }

    if( ui->cbDisplay->isChecked()){
        p.insert(QString("display-grid"), fullSliceName);
    }

    return p;
}

void CreateTimesliceDialog::updateOkButton(){

    bool ok=true;

    if( ui->leSliceName->text().isEmpty()){
        ok=false;
    }

    QPalette palette;
    if( reservedGrids().contains(ui->leSliceName->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leSliceName->setPalette(palette);

    if( !ui->cbUseHorizon->isChecked()){
        if( ui->leTime->text().isEmpty()){
            ok=false;
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

