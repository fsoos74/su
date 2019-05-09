#include "horizonitemsdialog.h"
#include "ui_horizonitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QMessageBox>
#include<QColorDialog>

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
    ui->sbOpacity->setValue(static_cast<int>(100*hitem->opacity()));
    ui->cbColor->setColor(hitem->color());
    ui->sbWidth->setValue(hitem->width());
}

void HorizonItemsDialog::on_pbAdd_clicked()
{
    int row=ui->lwAvailable->currentRow();
    if(row<0) return;

    auto name=ui->lwAvailable->currentItem()->text();
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
}

void HorizonItemsDialog::on_pbRemove_clicked()
{
    int row=ui->lwDisplayed->currentRow();
    if(row<0) return;
    auto item=ui->lwDisplayed->takeItem(row);
    delete item;
    mModel->remove(row);
}

void HorizonItemsDialog::on_pbMoveUp_clicked()
{
    auto idx=ui->lwDisplayed->currentRow();
    mModel->moveUp(idx);
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    ui->lwDisplayed->setCurrentRow(std::max(0,idx-1));
}

void HorizonItemsDialog::on_pbMoveDown_clicked()
{
    auto idx=ui->lwDisplayed->currentRow();
    mModel->moveDown(idx);
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    ui->lwDisplayed->setCurrentRow(std::min(idx+1, ui->lwDisplayed->count()-1));
}

void HorizonItemsDialog::on_sbOpacity_valueChanged(int arg1)
{
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto hitem=dynamic_cast<HorizonItem*>(mModel->at(idx));
    if(!hitem) return;

    auto opacity=arg1;
    hitem->setOpacity(opacity);
}

void HorizonItemsDialog::on_cbColor_clicked()
{
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto hitem=dynamic_cast<HorizonItem*>(mModel->at(idx));
    if(!hitem) return;

    auto color=QColorDialog::getColor(hitem->color(),this,"Select horizon color");
    if(color.isValid()){
        hitem->setColor(color);
        ui->cbColor->setColor(color);
    }
}

void HorizonItemsDialog::on_sbWidth_valueChanged(int arg1)
{
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto hitem=dynamic_cast<HorizonItem*>(mModel->at(idx));
    if(!hitem) return;

    hitem->setWidth(arg1);
}
