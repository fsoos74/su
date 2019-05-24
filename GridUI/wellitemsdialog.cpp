#include "wellitemsdialog.h"
#include "ui_wellitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QMessageBox>
#include<QColorDialog>
#include<QListWidget>
#include<QDebug>

namespace sliceviewer {


WellItemsDialog::WellItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WellItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);
    for(auto uwi : mProject->wellList()){
        auto winfo=mProject->getWellInfo(uwi);
        auto name=tr("%1|%2").arg(uwi,winfo.name());
        ui->lwAvailable->addItem(name);
    }
    ui->lwDisplayed->addItems(mModel->names());

    ui->cbLabelStyle->addItem("No label", static_cast<int>(WellItem::LabelStyle::NO_LABEL));
    ui->cbLabelStyle->addItem("Name", static_cast<int>(WellItem::LabelStyle::NAME_LABEL));
    ui->cbLabelStyle->addItem("UWI", static_cast<int>(WellItem::LabelStyle::UWI_LABEL));
}

WellItemsDialog::~WellItemsDialog()
{
    delete ui;
}

void WellItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    if(currentRow<0) return;
    auto witem=dynamic_cast<WellItem*>(mModel->at(currentRow));
    if(!witem) return;
    ui->sbOpacity->setValue(witem->opacity());
    ui->cbColor->setColor(witem->color());
    ui->sbWidth->setValue(witem->width());
    ui->cbLabelStyle->setCurrentIndex(ui->cbLabelStyle->findData(static_cast<int>(witem->labelStyle())));
}

void WellItemsDialog::on_pbAdd_clicked()
{
    mModel->setMute(true);
    for( auto midx : ui->lwAvailable->selectionModel()->selectedRows()){
        auto lwitem=ui->lwAvailable->item(midx.row());
        if(!lwitem) continue;
        auto name=lwitem->text();
        auto uwiAndName=name.split("|", QString::SkipEmptyParts);
        if(uwiAndName.size()<1){
            qDebug()<<"No uwi in "<<name<<"\n";
            continue;
        }
        auto uwi=uwiAndName[0];
        auto wellPath=mProject->loadWellPath(uwi);
        if( !wellPath){
            QMessageBox::critical(this, "Add Well", tr("Loading Well %1 failed!").arg(name));
            return;
        }

        WellItem* witem=new WellItem(this);
        witem->setName(name);
        witem->setUwi(uwi);
        witem->setWellPath(wellPath);
        witem->setLabelStyle(static_cast<WellItem::LabelStyle>(ui->cbLabelStyle->currentData().toInt()));
        mModel->add(witem);
        name=witem->name(); // name could have been updated to make it unique
        ui->lwDisplayed->addItem(name);
        ui->lwDisplayed->setCurrentRow(mModel->size()-1);
    }
    mModel->setMute(false);
    ui->lwAvailable->clearSelection();  // prevent accidentally multiple adding
}

void WellItemsDialog::on_pbRemove_clicked()
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

void WellItemsDialog::on_pbMoveUp_clicked()
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

void WellItemsDialog::on_pbMoveDown_clicked()
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

void WellItemsDialog::on_cbColor_clicked()
{
    auto color=QColorDialog::getColor(ui->cbColor->color(),this,"Select Well color");
    if(!color.isValid()) return;
    ui->cbColor->setColor(color);
}

void WellItemsDialog::on_pbApply_clicked()
{
    mModel->setMute(true);
    auto color=ui->cbColor->color();
    auto width=ui->sbWidth->value();
    auto opacity=ui->sbOpacity->value();
    auto labelStyle=static_cast<WellItem::LabelStyle>(ui->cbLabelStyle->currentData().toInt());
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto hitem=dynamic_cast<WellItem*>(mModel->at(row));
        if(!hitem) continue;
        hitem->setColor(color);
        hitem->setWidth(width);
        hitem->setOpacity(opacity);
        hitem->setLabelStyle(labelStyle);
    }
    mModel->setMute(false);
}


}
