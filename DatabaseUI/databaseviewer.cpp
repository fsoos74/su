#include "databaseviewer.h"
#include "ui_databaseviewer.h"

#include<QMessageBox>
#include<QHeaderView>
#include<iostream>

DatabaseViewer::DatabaseViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseViewer)
{
    ui->setupUi(this);

    setAttribute( Qt::WA_DeleteOnClose);

}

DatabaseViewer::~DatabaseViewer()
{
    delete ui;
}


void DatabaseViewer::setData( QSqlTableModel* table){

    ui->tableView->setModel(table);
    int w=0;
    for( int i=0; i<table->columnCount(); i++){

        w+=ui->tableView->columnWidth(i);
    }
    w+=ui->tableView->verticalHeader()->width();
    ui->tableView->setMinimumWidth(w);

}
