#include "markeritem.h"

MarkerItem::MarkerItem(QObject* parent) : ViewItem(parent), mColor(Qt::red), mWidth(2), mOpacity(100)
{

}

void MarkerItem::setPosition(QVector3D position){
    if(position==mPosition) return;
    mPosition=position;
    emit changed();
}

void MarkerItem::setUwi(QString uwi){
    if(uwi==mUwi) return;
    mUwi=uwi;
    emit changed();
}

void MarkerItem::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
    emit changed();
}

void MarkerItem::setWidth(int width){
    if(width==mWidth) return;
    mWidth=width;
    emit changed();
}

void MarkerItem::setOpacity(int opacity){
    if(opacity==mOpacity) return;
    opacity=mOpacity;
    return;
}
