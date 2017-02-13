#include "overlaypercentagedialog.h"
#include "ui_overlaypercentagedialog.h"

#include<QIntValidator>

OverlayPercentageDialog::OverlayPercentageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OverlayPercentageDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    validator->setRange(0, 100);
    ui->lePercentage->setValidator(validator);

    connect( ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(hide()) );
}

OverlayPercentageDialog::~OverlayPercentageDialog()
{
    delete ui;
}


int OverlayPercentageDialog::percentage(){

    return ui->slPercentage->value();
}


void OverlayPercentageDialog::setPercentage(int p){

    if( p==percentage()) return;

    ui->slPercentage->setValue(p);

    emit percentageChanged(p);
}

void OverlayPercentageDialog::on_slPercentage_valueChanged(int value)
{
    ui->lePercentage->setText(QString::number(value));
    emit percentageChanged(value);
}
