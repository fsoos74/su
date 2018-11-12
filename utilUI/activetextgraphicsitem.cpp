#include "activetextgraphicsitem.h"

#include<QPainter>

ActiveTextGraphicsItem::ActiveTextGraphicsItem( QString text, size_t id, QGraphicsItem* parent)
        : AlignedTextGraphicsItem( text, Qt::AlignCenter, 0, parent), m_id(id){

    setAcceptHoverEvents(true);
}


void ActiveTextGraphicsItem::setId(int i){
    m_id=i;
}

void ActiveTextGraphicsItem::setCallback( std::function<void(int)> f){
    m_callback=f;
}

void ActiveTextGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *){
    m_active=true;
    update();
}

void ActiveTextGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* ){
    m_active=false;
    update();
}


void ActiveTextGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *){
    if( m_callback ){
        m_callback(m_id);
    }
}

void ActiveTextGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w){

    if( m_active ){
        painter->setPen(QPen(Qt::red, 0));
        painter->drawRect(boundingRect());
    }
    else{
        painter->setPen(QPen(Qt::black,0));
    }

    AlignedTextGraphicsItem::paint(painter, o, w);
}

