#include "markeritemsdialog.h"
#include "ui_markeritemsdialog.h"
#include <avoproject.h>
#include<QMessageBox>
#include<QColorDialog>
#include<QListWidget>
#include<topsdbmanager.h>
#include<wellpath.h>


MarkerItemsDialog::MarkerItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarkerItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);
    auto db=mProject->openTopsDatabase();
    if(db){
        ui->lwAvailable->addItems(db->names());
    }
    ui->lwDisplayed->addItems(mModel->names());
}

MarkerItemsDialog::~MarkerItemsDialog()
{
    delete ui;
}

void MarkerItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    if(currentRow<0) return;
    auto mitem=dynamic_cast<MarkerItem*>(mModel->at(currentRow));
    if(!mitem) return;
    ui->sbOpacity->setValue(mitem->opacity());
    ui->cbColor->setColor(mitem->color());
    ui->sbWidth->setValue(mitem->width());
}

void MarkerItemsDialog::on_pbAdd_clicked()
{
    mModel->setMute(true);
    auto mdb=mProject->openTopsDatabase();
    for( auto midx : ui->lwAvailable->selectionModel()->selectedRows()){
        auto lwitem=ui->lwAvailable->item(midx.row());
        if(!lwitem) continue;
        auto name=lwitem->text();
        // iterate over wells
        auto mm=mdb->markersByName(name);
        for( auto wm : mm){
            auto mname=QString("%1(%2)").arg(wm.name(), wm.uwi());
            auto wp=mProject->loadWellPath(wm.uwi());
            if(!wp) continue;
            auto z=wp->zAtMD(wm.md());
            auto location=wp->locationAtZ(z);
            auto position=QVector3D(location.x(), location.y(), z);
            MarkerItem* mitem=new MarkerItem(this);
            mitem->setName(mname);
            mModel->add(mitem);
            mitem->setPosition(position);
            ui->lwDisplayed->addItem(mname);
            ui->lwDisplayed->setCurrentRow(mModel->size()-1);
            mitem->setColor(ui->cbColor->color());
            mitem->setOpacity(ui->sbOpacity->value());
            mitem->setWidth(ui->sbWidth->value());
        }
    }
    mModel->setMute(false);
    ui->lwAvailable->clearSelection();  // prevent accidentally multiple adding
}

void MarkerItemsDialog::on_pbRemove_clicked()
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

void MarkerItemsDialog::on_pbMoveUp_clicked()
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

void MarkerItemsDialog::on_pbMoveDown_clicked()
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

void MarkerItemsDialog::on_cbColor_clicked()
{
    auto color=QColorDialog::getColor(ui->cbColor->color(),this,"Select Marker color");
    if(!color.isValid()) return;
    ui->cbColor->setColor(color);
}

void MarkerItemsDialog::on_pbApply_clicked()
{
    mModel->setMute(true);
    auto color=ui->cbColor->color();
    auto width=ui->sbWidth->value();
    auto opacity=ui->sbOpacity->value();
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto hitem=dynamic_cast<MarkerItem*>(mModel->at(row));
        if(!hitem) continue;
        hitem->setColor(color);
        hitem->setWidth(width);
        hitem->setOpacity(opacity);
    }
    mModel->setMute(false);
}
