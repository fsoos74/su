#include "scatterplotviewerdisplayoptionsdialog.h"
#include "ui_scatterplotviewerdisplayoptionsdialog.h"

#include<QColorDialog>

ScatterplotViewerDisplayOptionsDialog::ScatterplotViewerDisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScatterplotViewerDisplayOptionsDialog)
{
    ui->setupUi(this);

    connect( ui->sbSize, SIGNAL(valueChanged(int)),
             this, SIGNAL(pointSizeChanged(int)) );

    connect( ui->cbOutline, SIGNAL(toggled(bool)),
             this, SIGNAL(pointOutlineChanged(bool)));
}

ScatterplotViewerDisplayOptionsDialog::~ScatterplotViewerDisplayOptionsDialog()
{
    delete ui;
}

int ScatterplotViewerDisplayOptionsDialog::pointSize(){
    return ui->sbSize->value();
}

bool ScatterplotViewerDisplayOptionsDialog::pointOutline(){

    return ui->cbOutline->isChecked();
}


void ScatterplotViewerDisplayOptionsDialog::setPointSize(int size){

  ui->sbSize->setValue(size);
}


void ScatterplotViewerDisplayOptionsDialog::setPointOutline(bool on){

    ui->cbOutline->setChecked(on);
}

