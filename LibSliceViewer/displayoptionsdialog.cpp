#include "displayoptionsdialog.h"
#include "ui_displayoptionsdialog.h"
#include <QColorDialog>

namespace sliceviewer{


DisplayOptionsDialog::DisplayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayOptionsDialog)
{
    ui->setupUi(this);

    ui->cbInlineOrientation->addItem("Vertical", static_cast<int>(Qt::Orientation::Vertical));
    ui->cbInlineOrientation->addItem("Horizontal", static_cast<int>(Qt::Orientation::Horizontal));
}

DisplayOptionsDialog::~DisplayOptionsDialog()
{
    delete ui;
}

DisplayOptions DisplayOptionsDialog::displayOptions(){
    DisplayOptions opts;
    opts.setDisplayVolumes(ui->cbDisplayVolumes->isChecked());
    opts.setDisplayHorizons(ui->cbDisplayHorizons->isChecked());
    opts.setDisplayWells(ui->cbDisplayWells->isChecked());
    opts.setDisplayMarkers(ui->cbDisplayMarkers->isChecked());
    opts.setDisplayTables(ui->cbDisplayTables->isChecked());
    opts.setDisplayLastViewed(ui->cbDisplayLastViewed->isChecked());
    opts.setLastViewedColor(ui->cbLastViewedColor->color());
    opts.setWellVisibilityDistance(ui->sbWellVisibilityDistance->value());
    opts.setSharpen(ui->cbSharpen->isChecked());
    opts.setSharpenFilterSize(ui->sbFilterSize->value());
    opts.setSharpenFilterStrength(ui->sbFilterStrength->value());
    opts.setAntiAliasing(ui->cbAntiAliasing->isChecked());
    opts.setAutoOrientation(ui->cbAutoOrientation->isChecked());
    opts.setInlineOrientation(static_cast<Qt::Orientation>(ui->cbInlineOrientation->currentData().toInt()));
    return opts;
}

void DisplayOptionsDialog::setDisplayOptions(const DisplayOptions & opts){
    ui->cbDisplayVolumes->setChecked(opts.isDisplayVolumes());
    ui->cbDisplayHorizons->setChecked(opts.isDisplayHorizons());
    ui->cbDisplayWells->setChecked(opts.isDisplayWells());
    ui->cbDisplayMarkers->setChecked(opts.isDisplayMarkers());
    ui->cbDisplayTables->setChecked(opts.isDisplayTables());
    ui->cbDisplayLastViewed->setChecked(opts.isDisplayLastViewed());
    ui->cbLastViewedColor->setColor(opts.lastViewedColor());
    ui->sbWellVisibilityDistance->setValue(opts.wellVisibilityDistance());
    ui->cbSharpen->setChecked(opts.isSharpen());
    ui->sbFilterSize->setValue(opts.sharpenFilterSize());
    ui->sbFilterStrength->setValue(opts.sharpenFilterStrength());
    ui->cbAntiAliasing->setChecked(opts.isAntiliasing());
    ui->cbAutoOrientation->setChecked(opts.isAutoOrientation());
    ui->cbInlineOrientation->setCurrentIndex(
                ui->cbInlineOrientation->findData(static_cast<int>(opts.inlineOrientation())));
}

void DisplayOptionsDialog::on_cbLastViewedColor_clicked()
{
    auto color=QColorDialog::getColor(ui->cbLastViewedColor->color(), this, "Select color");
    if(color.isValid()){
        ui->cbLastViewedColor->setColor(color);
    }
}

}
