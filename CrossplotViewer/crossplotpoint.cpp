#include "crossplotpoint.h"

#include<QPainter>


CrossplotPoint::CrossplotPoint(QGraphicsItem* parent) : QGraphicsItem(parent){

}

CrossplotPoint::Style CrossplotPoint::style()const{
    return mStyle;
}

void CrossplotPoint::setStyle(Style s){
    if(s!=mStyle){
        mStyle=s;
        update();
    }
}

int CrossplotPoint::size()const{
    return mSize;
}

void CrossplotPoint::setSize(int s){
    if(s!=mSize){
        mSize=s;
        update();
    }
}

QColor CrossplotPoint::color()const{
    return mColor;
}

void CrossplotPoint::setColor(QColor c){
    if(c!=mColor){
        mColor=c;
        update();
    }
}

QColor CrossplotPoint::selectedColor()const{
    return mSelectedColor;
}

void CrossplotPoint::setSelectedColor(QColor c){
    if(c!=mSelectedColor){
        mSelectedColor=c;
        update();
    }
}

QRectF CrossplotPoint::boundingRect()const{
    return QRectF(-mSize/2, -mSize/2, mSize, mSize);
}

void CrossplotPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*){
    painter->save();
    auto color=(isSelected()) ? mSelectedColor : mColor;

    switch(mStyle){
    case Style::Circle:
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(-mSize/2, -mSize/2, mSize, mSize);
        break;
    case Style::Square:
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->fillRect(boundingRect(), mColor);
        break;
    case Style::Cross:
        QPen pen(color, 2);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->drawLine(-mSize/2, -mSize/2, mSize/2, mSize/2);
        painter->drawLine(mSize/2, -mSize/2, -mSize/2, mSize/2);
        break;
    }
    painter->restore();
}
