#include "volumeitemsdialog.h"
#include "ui_volumeitemsdialog.h"
#include <avoproject.h>
#include<QIntValidator>
#include<QDoubleValidator>
#include<QMessageBox>
#include<colortabledialog.h>
#include<histogramcreator.h>
#include<histogramrangeselectiondialog.h>
#include<QColorDialog>

namespace sliceviewer {


VolumeItemsDialog::VolumeItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeItemsDialog),
    mProject(project),
    mModel(model)
{
    ui->setupUi(this);

    auto validator=new QDoubleValidator(this);
    ui->leGain->setValidator(validator);
    ui->cbStyle->addItem("Attribute",static_cast<int>(VolumeItem::Style::ATTRIBUTE));
    ui->cbStyle->addItem("Seismic",static_cast<int>(VolumeItem::Style::SEISMIC));
    ui->cbPolarity->addItem("Normal",static_cast<int>(VolumeItem::Polarity::NORMAL));
    ui->cbPolarity->addItem("Reversed",static_cast<int>(VolumeItem::Polarity::REVERSED));
    ui->lwAvailable->addItems(mProject->volumeList());
    ui->lwDisplayed->addItems(mModel->names());
}

VolumeItemsDialog::~VolumeItemsDialog()
{
    delete ui;
}

void VolumeItemsDialog::setCurrentItem(QString name){
    auto items=ui->lwDisplayed->findItems(name,Qt::MatchExactly);
    if(items.isEmpty()) return;
    auto item=items.front();
    ui->lwDisplayed->setCurrentItem(item);
}

void VolumeItemsDialog::on_lwDisplayed_currentRowChanged(int currentRow)
{
    if(currentRow<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(currentRow));
    if(!vitem) return;
    ui->sbOpacity->setValue(vitem->opacity());
    ui->cbStyle->setCurrentIndex(ui->cbStyle->findData(static_cast<int>(vitem->style())));
    ui->cbPolarity->setCurrentIndex(ui->cbPolarity->findData(static_cast<int>(vitem->polarity())));
    ui->leGain->setText(QString::number(vitem->gain()));
    ui->cbSeismic->setColor(vitem->seismicColor());
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
        HistogramCreator creator;
        auto histogram=creator.createHistogram(std::begin(*volume), std::end(*volume),
                                                          volume->NULL_VALUE,256);

        VolumeItem* vitem=new VolumeItem(this);
        vitem->setName(name);
        auto r=volume->valueRange();
        vitem->colorTable()->setRange(r);
        vitem->setVolume(volume);
        vitem->setHistogram(std::make_shared<Histogram>(histogram));

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

void VolumeItemsDialog::on_pbScaling_clicked(){
    mModel->setMute(true);
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto vitem=dynamic_cast<VolumeItem*>(mModel->at(row));
        if(!vitem) continue;
        auto ct=vitem->colorTable();
        if( !ct ) continue;

        /*
        if(!mHistogramBuffer.contains(vitem->name())){
            HistogramCreator hcreator;
            auto volume=vitem->volume().get();
            auto histogram=hcreator.createHistogram( QString("Volume %1").arg(vitem->name()), std::begin(*volume), std::end(*volume),
                                                 volume->NULL_VALUE, 100 );
            mHistogramBuffer.insert(vitem->name(), histogram);
        }
        auto histogram=mHistogramBuffer.value(vitem->name());
        */
        Q_ASSERT(vitem->histogram());
        auto histogram=*vitem->histogram();

        HistogramRangeSelectionDialog displayRangeDialog;
        displayRangeDialog.setWindowTitle(QString("Display Range - %1").arg(vitem->name()));
        displayRangeDialog.setRange(ct->range());
        displayRangeDialog.setColorTable(ct);       // all connections with ble are made by dialog
        displayRangeDialog.setHistogram(histogram);

        if( displayRangeDialog.exec()!=QDialog::Accepted){
            break;  // canceled, no more volumes
        }
   }
   mModel->setMute(false);
}

void VolumeItemsDialog::on_pbColors_clicked(){
    mModel->setMute(true);
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto vitem=dynamic_cast<VolumeItem*>(mModel->at(row));
        if(!vitem) continue;
        auto ct=vitem->colorTable();
        if( !ct ) continue;
        auto oldColors=ct->colors();
        auto dlg = new ColorTableDialog(oldColors, this);
        dlg->setWindowTitle(tr("Edit colortable volume \"%1\"").arg(vitem->name()));
        if(dlg->exec()!=QDialog::Accepted){
            ct->setColors(oldColors);
            break;   // canceled, process no more volumes
        }
        ct->setColors(dlg->colors());
    }
    mModel->setMute(false);
}

void VolumeItemsDialog::on_pbApply_clicked()
{
    mModel->setMute(true);
    auto style=static_cast<VolumeItem::Style>(ui->cbStyle->currentData().toInt());
    auto polarity=static_cast<VolumeItem::Polarity>(ui->cbPolarity->currentData().toInt());
    auto gain=ui->leGain->text().toDouble();
    auto opacity=ui->sbOpacity->value();
    auto seismicColor=ui->cbSeismic->color();
    for(auto mitem : ui->lwDisplayed->selectionModel()->selectedRows()){
        auto row=mitem.row();
        if(row<0) continue;
        auto vitem=dynamic_cast<VolumeItem*>(mModel->at(row));
        if(!vitem) continue;
        vitem->setStyle(style);
        vitem->setPolarity(polarity);
        vitem->setGain(gain);
        vitem->setOpacity(opacity);
        vitem->setSeismicColor(seismicColor);
    }
    mModel->setMute(false);
}


}

void sliceviewer::VolumeItemsDialog::on_cbSeismic_clicked()
{
    auto color=QColorDialog::getColor(ui->cbSeismic->color(),this,"Select Wiggle/VA color");
    if(!color.isValid()) return;
    ui->cbSeismic->setColor(color);
}
