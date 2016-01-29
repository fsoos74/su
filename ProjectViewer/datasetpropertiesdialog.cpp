#include "datasetpropertiesdialog.h"
#include "ui_datasetpropertiesdialog.h"

DatasetPropertiesDialog::DatasetPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatasetPropertiesDialog)
{
    ui->setupUi(this);

    ui->cbDomain->addItem( datasetDomainToString(SeismicDatasetInfo::Domain::Time) );
    ui->cbDomain->addItem( datasetDomainToString(SeismicDatasetInfo::Domain::Depth) );

    ui->cbMode->addItem( datasetModeToString(SeismicDatasetInfo::Mode::Prestack));
    ui->cbMode->addItem( datasetModeToString(SeismicDatasetInfo::Mode::Poststack));
}

DatasetPropertiesDialog::~DatasetPropertiesDialog()
{
    delete ui;
}


QString DatasetPropertiesDialog::name(){

    return ui->leName->text();
}

SeismicDatasetInfo::Domain DatasetPropertiesDialog::domain(){

   return stringToDatasetDomain( ui->cbDomain->currentText());
}

SeismicDatasetInfo::Mode DatasetPropertiesDialog::mode(){

    return stringToDatasetMode( ui->cbMode->currentText());
}

void DatasetPropertiesDialog::setName(const QString& name){

    ui->leName->setText( name );
}

void DatasetPropertiesDialog::setDomain(SeismicDatasetInfo::Domain domain){

    ui->cbDomain->setCurrentText( datasetDomainToString(domain) );
}

void DatasetPropertiesDialog::setMode(SeismicDatasetInfo::Mode mode){

    ui->cbMode->setCurrentText( datasetModeToString(mode));
}
