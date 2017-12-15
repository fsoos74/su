#include "editmarkersdialog.h"
#include "ui_editmarkersdialog.h"

EditMarkersDialog::EditMarkersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMarkersDialog)
{
    ui->setupUi(this);
}

EditMarkersDialog::~EditMarkersDialog()
{
    delete ui;
}


WellMarkers EditMarkersDialog::markers(){

    WellMarkers mks;
    auto model=ui->tableWidget->model();
    for( int i=0; i<model->rowCount(); i++){
        auto name = model->data(model->index(i,0)).toString();
        auto uwi = model->data(model->index(i,1)).toString();
        auto md = model->data(model->index(i,2)).toDouble();
        mks.push_back(WellMarker(name,uwi,md));
    }
    return mks;
}

void EditMarkersDialog::setMarkers(WellMarkers wms){

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(wms.size());
    ui->tableWidget->setColumnCount(3);
    auto model=ui->tableWidget->model();

    QStringList labels;
    labels<<"Name"<<"UWI"<<"MD";
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    for( int i = 0; i<wms.size(); i++){

        model->setData(model->index(i,0), wms[i].name());
        model->setData(model->index(i,1), wms[i].uwi());
        model->setData(model->index(i,2), wms[i].md());
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void EditMarkersDialog::on_pbAdd_clicked()
{
    auto row=ui->tableWidget->currentRow();
    ui->tableWidget->insertRow(row);
}

void EditMarkersDialog::on_pbRemove_clicked()
{
    auto row=ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(row);
}
