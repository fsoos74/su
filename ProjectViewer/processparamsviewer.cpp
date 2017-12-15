#include "processparamsviewer.h"
#include "ui_processparamsviewer.h"
#include <QStandardItemModel>

ProcessParamsViewer::ProcessParamsViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessParamsViewer)
{
    ui->setupUi(this);
}

ProcessParamsViewer::~ProcessParamsViewer()
{
    delete ui;
}


void ProcessParamsViewer::setParams(const ProcessParams& p){

    QStringList labels;
    labels<<"Parameter"<<"Value";

    auto model=new QStandardItemModel(p.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;

    for( auto name : p.keys() ){

        int column=0;

        model->setItem(row, column++, new QStandardItem(name) );
        model->setItem(row, column++, new QStandardItem(p.value(name) ) );
        row++;
    }

    ui->tableView->setModel(model);

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}
