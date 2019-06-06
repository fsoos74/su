#include "horizonitem.h"

namespace sliceviewer {


HorizonItem::HorizonItem(QObject *parent) : ViewItem(parent)
{

}

void HorizonItem::setHorizon(std::shared_ptr<Grid2D<float> > horizon){
    if(horizon==mHorizon) return;
    mHorizon=horizon;
    emit changed();
}

void HorizonItem::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
    emit changed();
}

void HorizonItem::setWidth(int width){
    if(width==mWidth) return;
    mWidth=width;
    emit changed();
}

void HorizonItem::setOpacity(int opacity){
    if(opacity==mOpacity) return;
    mOpacity=opacity;
    emit changed();
}

}
