#include "wellitem.h"

namespace sliceviewer {


WellItem::WellItem(QObject *parent) : ViewItem(parent),
    mColor(Qt::lightGray), mWidth(2), mOpacity(100),
    mLabelStyle(LabelStyle::NO_LABEL), mLabelFontSize(8)
{

}

void WellItem::setUwi(QString uwi){
    if(uwi==mUwi)return;
    mUwi=uwi;
    emit changed();
}

void WellItem::setWellName(QString wellName){
    if(wellName==mWellName) return;
    mWellName=wellName;
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

void WellItem::setLabelFontSize(int lfs){
    if(lfs==mLabelFontSize) return;
    mLabelFontSize=lfs;
    emit changed();
}

QString WellItem::label()const{
    QString label;
    switch(mLabelStyle){
    case LabelStyle::UWI_LABEL:
        label=mUwi;
        break;
    case LabelStyle::NAME_LABEL:
        label=mWellName;
        break;
    case LabelStyle::NAME_AND_UWI_LABEL:
        label=tr("%1|%2").arg(mWellName,mUwi);
        break;
    case LabelStyle::UWI_AND_NAME_LABEL:
        label=tr("%1|%2").arg(mUwi,mWellName);
        break;
    default:
        break;
    }

    return label;
}

}
