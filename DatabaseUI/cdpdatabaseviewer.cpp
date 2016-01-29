#include "cdpdatabaseviewer.h"
#include "ui_cdpdatabaseviewer.h"

#include<QSqlTableModel>

CDPDatabaseViewer::CDPDatabaseViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDPDatabaseViewer)
{
    ui->setupUi(this);
}

CDPDatabaseViewer::~CDPDatabaseViewer()
{
    delete ui;
}

void CDPDatabaseViewer::setDatabase( std::shared_ptr<CDPDatabase> db){
    m_db=db;
    updateModel();
}

void CDPDatabaseViewer::updateModel(){

    QSqlTableModel* model = new QSqlTableModel(this);
    model->setTable("cdps");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, tr("CDP"));
    model->setHeaderData(1, Qt::Horizontal, tr("Inline Number"));
    model->setHeaderData(2, Qt::Horizontal, tr("Crossline Number"));
    model->setHeaderData(3, Qt::Horizontal, tr("X Coordinate"));
    model->setHeaderData(4, Qt::Horizontal, tr("Y Coordinate"));

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // read only
    ui->tableView->resizeColumnsToContents();
}
