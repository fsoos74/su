#include "alignedtextgraphicsitem.h"


#include<QFontMetricsF>
#include<QPainter>
#include<cmath>

AlignedTextGraphicsItem::AlignedTextGraphicsItem( QString text, QGraphicsItem* parent)
    : QGraphicsItem( parent), m_text(text){
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);

}

void AlignedTextGraphicsItem::setText( QString& t){
    prepareGeometryChange();
    m_text=t;
    update();
}

void AlignedTextGraphicsItem::setFont(QFont font){
    prepareGeometryChange();
    m_font=font;
    update();
}

void AlignedTextGraphicsItem::setHAlign( Qt::Alignment a){
    // XXX need to check for valid values!!!
    prepareGeometryChange();
    m_halign=a;
    update();
}

void AlignedTextGraphicsItem::setVAlign( Qt::Alignment a){
    // XXX need to check for valid values!!!
    prepareGeometryChange();
    m_valign=a;
    update();
}

QRectF AlignedTextGraphicsItem::boundingRect()const{
    QFontMetricsF m(m_font);

    qreal x=0;
    qreal w=m.width(m_text);
    qreal y=0;
    qreal h=m.height();

    if( m_halign & Qt::AlignRight){
        x=-w;
    }
    if( m_halign & Qt::AlignHCenter){
        x=-w/2;
    }

    if( m_valign & Qt::AlignVCenter){
        y=m.descent()-m.ascent();
    }
    if( m_valign & Qt::AlignBottom){
        y=-h;
    }


    return QRectF(x, y, w, h);
}

#include<iostream>
void AlignedTextGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){


    painter->save();

    QTransform tf=transform();
    std::cout<<text().toStdString()<<" "<<"m11="<<tf.m11()<<" m22="<<tf.m22()<<std::endl<<std::flush;

    painter->scale(1./tf.m11(), 1./tf.m22());

    painter->drawText( boundingRect(), m_text);

    painter->restore();
}

