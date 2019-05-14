#include "wellitem.h"

WellItem::WellItem(QObject *parent) : ViewItem(parent), mColor(Qt::lightGray), mWidth(2), mOpacity(100)
{

}

void WellItem::setWellPath(std::shared_ptr<WellPath> wellPath){
    if(wellPath==mWellPath) return;
    mWellPath=wellPath;
    emit changed();
}

void WellItem::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
    emit changed();
}

void WellItem::setWidth(int width){
    if(width==mWidth) return;
    mWidth=width;
    emit changed();
}

void WellItem::setOpacity(int opacity){
    if(opacity==mOpacity) return;
    mOpacity=opacity;
    emit changed();
}
