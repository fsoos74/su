#include "multiitemselectiondialog.h"
#include "ui_multiitemselectiondialog.h"

QStringList MultiItemSelectionDialog::getItems(QWidget* parent, QString title, QString label, QStringList items, bool* ok, QStringList selection){
    MultiItemSelectionDialog dlg(parent);
    dlg.setWindowTitle(title);
    dlg.setLabel(label);
    dlg.setItems(items);
    dlg.setSelectedItems(selection);
    if( dlg.exec()==QDialog::Accepted){
        if(ok)*ok=true;
        return dlg.selectedItems();
    }
    else{
        if(ok)*ok=false;
        return QStringList();
    }
}

MultiItemSelectionDialog::MultiItemSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiItemSelectionDialog)
{
    ui->setupUi(this);
}

MultiItemSelectionDialog::~MultiItemSelectionDialog()
{
    delete ui;
}

QStringList MultiItemSelectionDialog::selectedItems(){
    auto ids = ui->listWidget->selectionModel()->selectedIndexes();
    QStringList res;
    for( auto mi : ids ){
        res<<ui->listWidget->item(mi.row())->text();
    }
    return res;
}

void MultiItemSelectionDialog::setLabel(QString text){
    ui->label->setText(text);
}

void MultiItemSelectionDialog::setItems(QStringList items){
    ui->listWidget->clear();
    ui->listWidget->addItems(items);
}

void MultiItemSelectionDialog::setSelectedItems(QStringList items){
    auto model=ui->listWidget->selectionModel();
    model->clear();
    for( int row=0; row<ui->listWidget->count(); row++){
        auto s = ui->listWidget->item(row)->text();
        if( items.contains(s)){
            auto index = ui->listWidget->model()->index(row,0);
            model->select(index, QItemSelectionModel::Select);
        }
    }
}

void MultiItemSelectionDialog::on_pbClear_clicked()
{
    ui->listWidget->clearSelection();
}
