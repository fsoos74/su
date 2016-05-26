#include "crossplotviewerdisplayoptionsdialog.h"
#include "ui_crossplotviewerdisplayoptionsdialog.h"

CrossplotViewerDisplayOptionsDialog::CrossplotViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrossplotViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbDatapointSize, SIGNAL(valueChanged(int)), this, SLOT(setDatapointSize(int)) );
}

CrossplotViewerDisplayOptionsDialog::~CrossplotViewerDisplayOptionsDialog()
{
    delete ui;
}

int CrossplotViewerDisplayOptionsDialog::datapointSize(){
    return ui->sbDatapointSize->value();
}


void CrossplotViewerDisplayOptionsDialog::setDatapointSize(int size){

    // do not compare new and old value, spinbox does this anyway and we need signal to be eimitted if this is triggered by spinbox
    ui->sbDatapointSize->setValue(size);

    emit datapointSizeChanged(size);
}

