#include "tracegaindialog.h"
#include "ui_tracegaindialog.h"

#include <QPushButton>
#include <QDoubleValidator>

TraceGainDialog::TraceGainDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::TraceGainDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leFactor->setValidator(validator);

    connect( ui->cbInputDataset, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leFactor, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
}

TraceGainDialog::~TraceGainDialog()
{
    delete ui;
}


void TraceGainDialog::setInputDatasets(const QStringList & l){

    ui->cbInputDataset->clear();
    ui->cbInputDataset->addItems(l);
    updateOkButton();
}


QMap<QString,QString> TraceGainDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-dataset"), ui->cbInputDataset->currentText() );

    p.insert( QString("factor"), QString::number(ui->leFactor->text().toDouble()));

    return p;
}

void TraceGainDialog::updateOkButton(){

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

