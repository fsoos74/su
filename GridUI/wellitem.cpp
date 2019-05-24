#include "wellitem.h"

namespace sliceviewer {


WellItem::WellItem(QObject *parent) : ViewItem(parent),
    mColor(Qt::lightGray), mWidth(2), mOpacity(100), mLabelStyle(LabelStyle::NO_LABEL)
{

}

void WellItem::setUwi(QString uwi){
    if(uwi==mUwi)return;
    mUwi=uwi;
    emit changed();
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

void WellItem::setLabelStyle(LabelStyle labelStyle){
    if(labelStyle==mLabelStyle) return;
    mLabelStyle=labelStyle;
    emit changed();
}

}
