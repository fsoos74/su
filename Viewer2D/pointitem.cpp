#include "pointitem.h"

#include "wellitem.h"

#include<QPen>
#include<QBrush>
#include<QFont>
#include<QVector2D>
#include<cmath>
#include<QMap>

PointItem::PointItem( QGraphicsItem* parent, QObject* parentObject) :
    QObject(parentObject),
    QGraphicsItemGroup(parent)
{
    m_symbol=new QGraphicsEllipseItem(-m_size,-m_size,2*m_size,2*m_size,this);
    m_symbol->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_symbol->setPen(Qt::NoPen);
    QBrush brush(Qt::darkGreen);
    m_symbol->setBrush(brush);

    m_label=new AlignedTextGraphicsItem("", Qt::AlignHCenter|Qt::AlignTop,0,this);
    QFont font("Times",8,QFont::Normal);
    m_label->setFont(font);
    m_label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_label->moveBy(0,m_size);
}


int PointItem::size()const{
    return m_size;
}

QFont PointItem::font()const{
    return m_label->font();
}

QString PointItem::text()const{
    return m_label->text();
}

void PointItem::setSize(int s){

    m_size=s;

    prepareGeometryChange();

    QRectF rItem( -s/2, -s/2, 2*s, 2*s);
    m_symbol->setRect(rItem);
    //setRect(rItem);
    m_label->setPos(m_size,m_size);
}

void PointItem::setSymbolColor(QColor c){
    m_symbolColor=c;
    QBrush brush(m_symbolColor);
    m_symbol->setBrush(brush);
    update();
}

void PointItem::setText(QString text){
    m_label->setText(text);
}

void PointItem::setFont(QFont f){
    m_label->setFont(f);
}
