#include "curveviewerdisplayoptionsdialog.h"
#include "ui_curveviewerdisplayoptionsdialog.h"

CurveViewerDisplayOptionsDialog::CurveViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurveViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbDatapointSize, SIGNAL(valueChanged(int)), this, SLOT( setDatapointSize(int)) );
}

CurveViewerDisplayOptionsDialog::~CurveViewerDisplayOptionsDialog()
{
    delete ui;
}

int CurveViewerDisplayOptionsDialog::datapointSize(){
    return ui->sbDatapointSize->value();
}


void CurveViewerDisplayOptionsDialog::setDatapointSize(int size){

   // if( size==datapointSize()) return;        // one extra round possible because spinbox will stop the cycle

    ui->sbDatapointSize->setValue(size);

    emit datapointSizeChanged(size);
}

