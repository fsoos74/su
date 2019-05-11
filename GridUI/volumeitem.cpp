#include "volumeitem.h"

#include<colortable.h>

VolumeItem::VolumeItem(QObject* parent) : ViewItem(parent),
    mColorTable(new ColorTable(this,ColorTable::defaultColors(),std::make_pair(0.,1.))),
    mOpacity(100),
    mStyle(Style::ATTRIBUTE)
{
    connect(mColorTable, SIGNAL(colorsChanged()), this, SIGNAL(changed()));
    connect(mColorTable, SIGNAL(rangeChanged(std::pair<double,double> )), this, SIGNAL(changed()));
    connect(mColorTable, SIGNAL(powerChanged(double)), this, SIGNAL(changed()));
}

void VolumeItem::setVolume(std::shared_ptr<Volume> volume){
    if(volume==mVolume) return;
    mVolume=volume;
    emit changed();
}

void VolumeItem::setOpacity(int opacity){
    if(opacity==mOpacity) return;
    mOpacity=opacity;
    emit changed();
}

void VolumeItem::setStyle(VolumeItem::Style style){
    if(style==mStyle) return;
    mStyle=style;
    emit changed();
}
