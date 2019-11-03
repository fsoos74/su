#include "offsetcomputationdialog.h"
#include "ui_offsetcomputationdialog.h"

#include <QPushButton>
#include <QDoubleValidator>

OffsetComputationDialog::OffsetComputationDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::OffsetComputationDialog)
{
    ui->setupUi(this);
    connect( ui->cbInputDataset, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

OffsetComputationDialog::~OffsetComputationDialog()
{
    delete ui;
}


void OffsetComputationDialog::setInputDatasets(const QStringList & l){

    ui->cbInputDataset->clear();
    ui->cbInputDataset->addItems(l);
    updateOkButton();
}


QMap<QString,QString> OffsetComputationDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-dataset"), ui->cbInputDataset->currentText() );

    return p;
}

void OffsetComputationDialog::updateOkButton(){

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

