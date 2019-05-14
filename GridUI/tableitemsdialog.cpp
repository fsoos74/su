#include "tableitemsdialog.h"
#include "ui_tableitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QMessageBox>
#include<QColorDialog>
#include<QListWidget>

TableItemsDialog::TableItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);
    ui->lwAvailable->addItems(mProject->tableList());
    ui->lwDisplayed->addItems(mModel->names());
}

TableItemsDialog::~TableItemsDialog()
{
    delete ui;
}

void TableItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    if(currentRow<0) return;
    auto witem=dynamic_cast<TableItem*>(mModel->at(currentRow));
    if(!witem) return;
    ui->sbOpacity->setValue(witem->opacity());
    ui->cbColor->setColor(witem->color());
    ui->sbPointSize->setValue(witem->pointSize());
}

void TableItemsDialog::on_pbAdd_clicked()
{
    mModel->setMute(true);
    for( auto midx : ui->lwAvailable->selectionModel()->selectedRows()){
        auto lwitem=ui->lwAvailable->item(midx.row());
        if(!lwitem) continue;
        auto name=lwitem->text();
        auto table=mProject->loadTable(name);
        if( !table){
            QMessageBox::critical(this, "Add Table", tr("Loading table %1 failed!").arg(name));
            return;
        }

        TableItem* witem=new TableItem(this);
        witem->setName(name);
        witem->setTable(table);
        mModel->add(witem);
        name=witem->name(); // name could have been updated to make it unique
        ui->lwDisplayed->addItem(name);
        ui->lwDisplayed->setCurrentRow(mModel->size()-1);
    }
    mModel->setMute(false);
    ui->lwAvailable->clearSelection();  // prevent accidentally multiple adding
}

void TableItemsDialog::on_pbRemove_clicked()
{
    auto midcs=ui->lwDisplayed->selectionModel()->selectedRows();
    // remove items in reverse order to keep indices valid
    std::sort(std::begin(midcs), std::end(midcs),
           [](const QModelIndex& m1, const QModelIndex& m2)->bool{return m2.row()<m1.row();} );
    mModel->setMute(true);
    for( QModelIndex midx : midcs){
        auto row=midx.row();
        auto item=ui->lwDisplayed->takeItem(row);
        delete item;
        mModel->remove(row);
    }
    mModel->setMute(false);
}

void TableItemsDialog::on_pbMoveUp_clicked()
{
    // move items in  order
    auto midcs=ui->lwDisplayed->selectionModel()->selectedRows();
    std::sort(std::begin(midcs), std::end(midcs),
           [](const QModelIndex& m1, const QModelIndex& m2)->bool{return m1.row()<m2.row();} );
    mModel->setMute(true);
    QVector<QModelIndex> newSelection;
    for( QModelIndex midx : midcs){
        auto row=midx.row();
        mModel->moveUp(row);
        if(row>0){
            newSelection.append(ui->lwDisplayed->model()->index(row-1,0));
        }
    }
    // refresh items list
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    // update selection
    ui->lwDisplayed->selectionModel()->clearSelection();
    for(auto midx : newSelection){
        ui->lwDisplayed->selectionModel()->select(midx, QItemSelectionModel::Select);
    }
    mModel->setMute(false);     // finally update view
}

void TableItemsDialog::on_pbMoveDown_clicked()
{
    // move items in  reverse order
    auto midcs=ui->lwDisplayed->selectionModel()->selectedRows();
    std::sort(std::begin(midcs), std::end(midcs),
           [](const QModelIndex& m1, const QModelIndex& m2)->bool{return m1.row()>m2.row();} );
    mModel->setMute(true);
    QVector<QModelIndex> newSelection;
    for( QModelIndex midx : midcs){
        auto row=midx.row();
        mModel->moveDown(row);
        if(row+1<mModel->size()){
            newSelection.append(ui->lwDisplayed->model()->index(row+1,0));
        }
    }
    // refresh items list
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    // update selection
    ui->lwDisplayed->selectionModel()->clearSelection();
    for(auto midx : newSelection){
        ui->lwDisplayed->selectionModel()->select(midx, QItemSelectionModel::Select);
    }
    mModel->setMute(false); // update views
}

void TableItemsDialog::on_cbColor_clicked()
{
    auto color=QColorDialog::getColor(ui->cbColor->color(),this,"Select Table color");
    if(!color.isValid()) return;
    ui->cbColor->setColor(color);
}

void TableItemsDialog::on_pbApply_clicked()
{
    mModel->setMute(true);
    auto color=ui->cbColor->color();
    auto pointSize=ui->sbPointSize->value();
    auto opacity=ui->sbOpacity->value();
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto hitem=dynamic_cast<TableItem*>(mModel->at(row));
        if(!hitem) continue;
        hitem->setColor(color);
        hitem->setPointSize(pointSize);
        hitem->setOpacity(opacity);
    }
    mModel->setMute(false);
}
