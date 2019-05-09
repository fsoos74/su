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
    if(currentRow<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(currentRow));
    if(!vitem) return;
    ui->sbOpacity->setValue(static_cast<int>(100*vitem->opacity()));
    int idx=ui->cbStyle->findData(static_cast<int>(vitem->style()));
    if(idx<0) return;
    ui->cbStyle->setCurrentIndex(idx);
}

void VolumeItemsDialog::on_pbAdd_clicked()
{
    int row=ui->lwAvailable->currentRow();
    if(row<0) return;

    auto name=ui->lwAvailable->currentItem()->text();
    auto volume=mProject->loadVolume(name, true);
    if( !volume){
        QMessageBox::critical(this, "Add Volume", "Loading volume failed!");
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
}

void VolumeItemsDialog::on_pbRemove_clicked()
{
    int row=ui->lwDisplayed->currentRow();
    if(row<0) return;
    auto item=ui->lwDisplayed->takeItem(row);
    delete item;
    mModel->remove(row);
}

void VolumeItemsDialog::on_pbMoveUp_clicked()
{
    auto idx=ui->lwDisplayed->currentRow();
    mModel->moveUp(idx);
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    ui->lwDisplayed->setCurrentRow(std::max(0,idx-1));
}

void VolumeItemsDialog::on_pbMoveDown_clicked()
{
    auto idx=ui->lwDisplayed->currentRow();
    mModel->moveDown(idx);
    ui->lwDisplayed->clear();
    ui->lwDisplayed->addItems(mModel->names());
    ui->lwDisplayed->setCurrentRow(std::min(idx+1, ui->lwDisplayed->count()-1));
}

void VolumeItemsDialog::on_pbColors_clicked(){

    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
     auto vitem=dynamic_cast<VolumeItem*>(mModel->at(idx));
    if(!vitem) return;

    auto ct=vitem->colorTable();
    QVector<QRgb> oldColors=ct->colors();

    ColorTableDialog colorTableDialog( oldColors);
    colorTableDialog.setWindowTitle(QString("Colortable - %1").arg(vitem->name()));

    if( colorTableDialog.exec()==QDialog::Accepted ){
        ct->setColors( colorTableDialog.colors());
    }else{
        ct->setColors( oldColors );
    }
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
    displayRangeDialog.setColorTable(ct);       // all connections with colortable are made by dialog
    displayRangeDialog.setHistogram(histogram);

    displayRangeDialog.exec();
}

void VolumeItemsDialog::on_sbOpacity_valueChanged(int arg1)
{
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(idx));
    if(!vitem) return;

    auto opacity=0.01*arg1;      // percent -> fraction
    vitem->setOpacity(opacity);
}

void VolumeItemsDialog::on_cbStyle_currentIndexChanged(int)
{
    auto idx=ui->lwDisplayed->currentRow();
    if(idx<0) return;
    auto vitem=dynamic_cast<VolumeItem*>(mModel->at(idx));
    if(!vitem) return;

    vitem->setStyle(static_cast<VolumeItem::Style>(ui->cbStyle->currentData().toInt()));
}
