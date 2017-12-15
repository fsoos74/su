#include "replacebadtracesdialog.h"
#include "ui_replacebadtracesdialog.h"

#include <QPushButton>
#include <QDoubleValidator>

ReplaceBadTracesDialog::ReplaceBadTracesDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ReplaceBadTracesDialog)
{
    ui->setupUi(this);

    connect( ui->cbInputDataset, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

ReplaceBadTracesDialog::~ReplaceBadTracesDialog()
{
    delete ui;
}


void ReplaceBadTracesDialog::setInputDatasets(const QStringList & l){

    ui->cbInputDataset->clear();
    ui->cbInputDataset->addItems(l);
    updateOkButton();
}


QMap<QString,QString> ReplaceBadTracesDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-dataset"), ui->cbInputDataset->currentText() );


    return p;
}

void ReplaceBadTracesDialog::updateOkButton(){

    bool ok=true;

    if( ui->leOutputDataset->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( reservedDatasets().contains( ui->leOutputDataset->text() ) ) {
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leOutputDataset->setPalette(palette);
    }


    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

