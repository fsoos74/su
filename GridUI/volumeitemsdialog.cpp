#include "volumeitemsdialog.h"
#include "ui_volumeitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QMessageBox>
#include<colortabledialog.h>
#include<histogramcreator.h>
#include<histogramrangeselectiondialog.h>

VolumeItemsDialog::VolumeItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);

    auto ivalidator=new QIntValidator(this);
    ivalidator->setRange(0,100);

    ui->cbStyle->addItem("Attribute",static_cast<int>(VolumeItem::Style::ATTRIBUTE));
    ui->cbStyle->addItem("Seismic",static_cast<int>(VolumeItem::Style::SEISMIC));

    ui->lwAvailable->addItems(mProject->volumeList());
    ui->lwDisplayed->addItems(mModel->names());
}

VolumeItemsDialog::~VolumeItemsDialog()
{
    delete ui;
}

void VolumeItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    std::cout<<"current row changed "<<currentRow<<std::endl<<std::flush;
    if(currentRow<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(currentRow));
    if(!vitem) return;
    ui->sbOpacity->setValue(vitem->opacity());
    int idx=ui->cbStyle->findData(static_cast<int>(vitem->style()));
    if(idx<0) return;
    ui->cbStyle->setCurrentIndex(idx);
}

void VolumeItemsDialog::on_pbAdd_clicked()
{
    mModel->setMute(true);
    for( auto midx : ui->lwAvailable->selectionModel()->selectedRows()){
        auto lwitem=ui->lwAvailable->item(midx.row());
        if(!lwitem) continue;
        auto name=lwitem->text();
        auto volume=mProject->loadVolume(name, true);
        if( !volume){
            QMessageBox::critical(this, "Add Volume", QString("Loading volume \"%1\" failed!").arg(name));
            return;
        }

        VolumeItem* vitem=new VolumeItem(this);
        vitem->setName(name);
        auto r=volume->valueRange();
        vitem->colorTable()->setRange(r);
        vitem->setVolume(volume);

        mModel->add(vitem);
        name=vitem->name(); // name could have been updated to make it unique
        ui->lwDisplayed->addItem(name);
        ui->lwDisplayed->setCurrentRow(mModel->size()-1);
    }
    mModel->setMute(false);

    ui->lwAvailable->clearSelection();  // prevent accidentally multiple adding
}

void VolumeItemsDialog::on_pbRemove_clicked()
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

void VolumeItemsDialog::on_pbMoveUp_clicked()
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

void VolumeItemsDialog::on_pbMoveDown_clicked()
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

void VolumeItemsDialog::on_sbOpacity_valueChanged(int arg1)
{
    auto opacity=arg1;
    auto row = ui->lwDisplayed->currentRow();
    if(row<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(row));
    if(!vitem) return;
    vitem->setOpacity(opacity);
}

void VolumeItemsDialog::on_pbScaling_clicked(){
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(idx));
    if(!vitem) return;

    auto ct=vitem->colorTable();
    if( !ct ) return;

    HistogramCreator hcreator;
    //connect( &hcreator, SIGNAL(percentDone(int)), pbar, SLOT(setValue(int)) );
    auto volume=vitem->volume().get();
    auto histogram=hcreator.createHistogram( QString("Volume %1").arg(vitem->name()), std::begin(*volume), std::end(*volume), volume->NULL_VALUE, 100 );

    HistogramRangeSelectionDialog displayRangeDialog;
    displayRangeDialog.setWindowTitle(QString("Display Range - %1").arg(vitem->name()));
    displayRangeDialog.setRange(ct->range());
    displayRangeDialog.setColorTable(ct);       // all connections with ble are made by dialog
    displayRangeDialog.setHistogram(histogram);

    displayRangeDialog.exec();
}

void VolumeItemsDialog::on_pbColors_clicked(){
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(idx));
    if(!vitem) return;

    auto ct=vitem->colorTable();
    if( !ct ) return;

    auto oldColors=ct->colors();
    auto dlg = new ColorTableDialog(oldColors, this);
    dlg->setWindowTitle(QString("Edit colortable volume \"%1\"").arg(vitem->name()));
    if(dlg->exec()==QDialog::Accepted){
        ct->setColors(dlg->colors());
    }
}

void VolumeItemsDialog::on_cbStyle_currentIndexChanged(int)
{
   auto row = ui->lwDisplayed->currentRow();
   if(row<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(row));
    if(!vitem) return;
    vitem->setStyle(static_cast<VolumeItem::Style>(ui->cbStyle->currentData().toInt()));
}
