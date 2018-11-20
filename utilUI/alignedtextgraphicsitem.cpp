#include "alignedtextgraphicsitem.h"

#include<QPainter>


AlignedTextGraphicsItem::AlignedTextGraphicsItem(QString text,
             Qt::Alignment align, double rot, QGraphicsItem* parent):
    QGraphicsSimpleTextItem(text,parent),m_align(align),m_rot(rot){

}

QRectF AlignedTextGraphicsItem::boundingRect()const{
    auto metrics=QFontMetrics(font());
    auto br=metrics.boundingRect(text());
    int dx=0;
    int dy=0;
    if(m_align&Qt::AlignHCenter) dx-=br.width()/2;
    if(m_align&Qt::AlignRight) dx-=br.width();
    if(m_align&Qt::AlignVCenter) dy-=0;
    if(m_align&Qt::AlignTop) dy+=br.height()/2;
    if(m_align&Qt::AlignBottom) dy-=br.height()/2;
    br.translate(dx,dy);
    return br;
}

void AlignedTextGraphicsItem::paint(QPainter *painter,
        const QStyleOptionGraphicsItem *, QWidget *){

    painter->save();
    painter->setFont(font());
    auto br=boundingRect();
    painter->rotate(m_rot);
    painter->drawText(br.x(),br.y()+br.height(),text());
    painter->restore();
}
