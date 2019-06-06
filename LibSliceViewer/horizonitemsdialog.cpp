#include "horizonitemsdialog.h"
#include "ui_horizonitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QMessageBox>
#include<QColorDialog>
#include<QListWidget>


namespace sliceviewer {


HorizonItemsDialog::HorizonItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HorizonItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);

    auto ivalidator=new QIntValidator(this);
    ivalidator->setRange(0,100);

    ui->lwAvailable->addItems(mProject->gridList(GridType::Horizon));
    ui->lwDisplayed->addItems(mModel->names());
}

HorizonItemsDialog::~HorizonItemsDialog()
{
    delete ui;
}

void HorizonItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    if(currentRow<0) return;
    auto hitem=dynamic_cast<HorizonItem*>(mModel->at(currentRow));
    if(!hitem) return;
    ui->sbOpacity->setValue(hitem->opacity());
    ui->cbColor->setColor(hitem->color());
    ui->sbWidth->setValue(hitem->width());
}

void HorizonItemsDialog::on_pbAdd_clicked()
{
    mModel->setMute(true);
    for( auto midx : ui->lwAvailable->selectionModel()->selectedRows()){
        auto lwitem=ui->lwAvailable->item(midx.row());
        if(!lwitem) continue;
        auto name=lwitem->text();
        auto horizon=mProject->loadGrid(GridType::Horizon, name);
        if( !horizon){
            QMessageBox::critical(this, "Add Horizon", "Loading horizon failed!");
            return;
        }

        HorizonItem* hitem=new HorizonItem(this);
        hitem->setName(name);
        hitem->setHorizon(horizon);
        mModel->add(hitem);
        name=hitem->name(); // name could have been updated to make it unique
        ui->lwDisplayed->addItem(name);
        ui->lwDisplayed->setCurrentRow(mModel->size()-1);
    }
    mModel->setMute(false);
    ui->lwAvailable->clearSelection();  // prevent accidentally multiple adding
}

void HorizonItemsDialog::on_pbRemove_clicked()
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

void HorizonItemsDialog::on_pbMoveUp_clicked()
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

void HorizonItemsDialog::on_pbMoveDown_clicked()
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

void HorizonItemsDialog::on_cbColor_clicked()
{
    auto color=QColorDialog::getColor(ui->cbColor->color(),this,"Select horizon color");
    if(!color.isValid()) return;
    ui->cbColor->setColor(color);
}

void HorizonItemsDialog::on_pbApply_clicked()
{
    mModel->setMute(true);
    auto color=ui->cbColor->color();
    auto width=ui->sbWidth->value();
    auto opacity=ui->sbOpacity->value();
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto hitem=dynamic_cast<HorizonItem*>(mModel->at(row));
        if(!hitem) continue;
        hitem->setColor(color);
        hitem->setWidth(width);
        hitem->setOpacity(opacity);
    }
    mModel->setMute(false);
}


}
