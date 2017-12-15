#include "offsetstackdialog.h"
#include "ui_offsetstackdialog.h"

#include <QPushButton>
#include <QDoubleValidator>

OffsetStackDialog::OffsetStackDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::OffsetStackDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->leMinOffset->setValidator(validator);
    ui->leMaxOffset->setValidator(validator);

    connect( ui->cbInputDataset, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leOutputDataset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMinOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leMaxOffset, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

}

OffsetStackDialog::~OffsetStackDialog()
{
    delete ui;
}


void OffsetStackDialog::setInputDatasets(const QStringList & l){

    ui->cbInputDataset->clear();
    ui->cbInputDataset->addItems(l);
    updateOkButton();
}

void OffsetStackDialog::setOffsetRange(float min, float max){

    ui->leMinOffset->setText( QString::number(min));
    ui->leMaxOffset->setText( QString::number(max));
}

QMap<QString,QString> OffsetStackDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("output-dataset"), ui->leOutputDataset->text() );

    p.insert( QString("input-dataset"), ui->cbInputDataset->currentText() );

    p.insert( QString("min-offset"), QString::number(ui->leMinOffset->text().toDouble()));
    p.insert( QString("max-offset"), QString::number(ui->leMaxOffset->text().toDouble()));

    return p;
}

void OffsetStackDialog::updateOkButton(){

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

    // test offset range
    {
    QPalette palette;
    if( ui->leMinOffset->text().toFloat() > ui->leMaxOffset->text().toFloat() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinOffset->setPalette(palette);
    ui->leMaxOffset->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

