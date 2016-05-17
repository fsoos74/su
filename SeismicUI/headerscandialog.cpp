#include "headerscandialog.h"
#include "ui_headerscandialog.h"

HeaderScanDialog::HeaderScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HeaderScanDialog)
{
    ui->setupUi(this);

}

HeaderScanDialog::~HeaderScanDialog()
{
    delete ui;
}

void HeaderScanDialog::setModel(QStandardItemModel* model){

    ui->tableView->setModel(model);

    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->resizeColumnsToContents();

}
