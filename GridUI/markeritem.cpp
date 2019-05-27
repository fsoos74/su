#include "markeritem.h"

namespace sliceviewer {


MarkerItem::MarkerItem(QObject* parent) : ViewItem(parent),
    mColor(Qt::red), mWidth(2), mOpacity(100),
    mLabelStyle(MarkerItem::LabelStyle::NAME_LABEL),mLabelFontSize(8)
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

void MarkerItem::setMarkerName(QString markerName){
    if(markerName==mMarkerName) return;
    mMarkerName=markerName;
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

void MarkerItem::setLabelStyle(LabelStyle labelStyle){
    if(labelStyle==mLabelStyle) return;
    mLabelStyle=labelStyle;
    emit changed();
}

void MarkerItem::setLabelFontSize(int labelFontSize){
    if(labelFontSize==mLabelFontSize) return;
    mLabelFontSize=labelFontSize;
    emit changed();
}

QString MarkerItem::label()const{
    QString label;
    switch(mLabelStyle){
    case LabelStyle::UWI_LABEL:
        label=mUwi;
        break;
    case MarkerItem::LabelStyle::NAME_LABEL:
        label=mMarkerName;
        break;
    case MarkerItem::LabelStyle::NAME_AND_UWI_LABEL:
        label=tr("%1@%2").arg(mMarkerName,mUwi);
        break;
    default:
        break;
    }

    return label;
}

}
