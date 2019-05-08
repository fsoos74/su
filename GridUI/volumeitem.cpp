#include "volumeitem.h"

#include<colortable.h>

VolumeItem::VolumeItem(QObject* parent) : QObject(parent),mColorTable(new ColorTable(this,ColorTable::defaultColors(),std::make_pair(0.,1.))),mOpacity(1)
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

void VolumeItem::setOpacity(qreal opacity){
    if(opacity==mOpacity) return;
    mOpacity=opacity;
    emit changed();
}

void VolumeItem::setName(QString name){
    if(name==mName) return;
    mName=name;
    emit changed();
}

void VolumeItem::setStyle(VolumeItem::Style style){
    if(style==mStyle) return;
    mStyle=style;
    emit changed();
}
