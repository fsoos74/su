#include "crossplotviewerdisplayoptionsdialog.h"
#include "ui_crossplotviewerdisplayoptionsdialog.h"

CrossplotViewerDisplayOptionsDialog::CrossplotViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbDatapointSize, SIGNAL(valueChanged(int)), this, SLOT(setDatapointSize(int)) );
    connect( ui->cbFlattenTrend, SIGNAL(toggled(bool)), this, SLOT(setFlattenTrend(bool)) );
}

CrossplotViewerDisplayOptionsDialog::~CrossplotViewerDisplayOptionsDialog()
{
    delete ui;
}

int CrossplotViewerDisplayOptionsDialog::datapointSize(){
    return ui->sbDatapointSize->value();
}

bool CrossplotViewerDisplayOptionsDialog::flattenTrend(){

    return ui->cbFlattenTrend->isChecked();
}


void CrossplotViewerDisplayOptionsDialog::setDatapointSize(int size){

    // do not compare new and old value, spinbox does this anyway and we need signal to be eimitted if this is triggered by spinbox
    ui->sbDatapointSize->setValue(size);

    emit datapointSizeChanged(size);
}

void CrossplotViewerDisplayOptionsDialog::setFlattenTrend(bool on){

    // do not compare new and old value, checkbox does this anyway and we need signal to be eimitted if this is triggered by checkbox
    ui->cbFlattenTrend->setChecked(on);

    emit flattenTrendChanged(on);
}

