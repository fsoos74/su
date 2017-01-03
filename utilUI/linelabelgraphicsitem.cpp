#include "linelabelgraphicsitem.h"

#include<QPainter>
#include<QSet>
#include<cmath>
#include<QDebug>
#include<iostream>

LineLabelGraphicsItem::LineLabelGraphicsItem(QLineF line, QString text, QFont font ) :
    QGraphicsItem(), m_line(line),  m_text(text), m_font(font){


    updateLine();
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);
}


void LineLabelGraphicsItem::updateLine(){
    setPos(m_line.p1());
    setRotation(-m_line.angle());
}

void LineLabelGraphicsItem::setLine(QLineF l){

    if( l==m_line) return;

    prepareGeometryChange();

    m_line=l;

    updateLine();
}


void LineLabelGraphicsItem::setText(QString t){

    if( t==m_text ) return;

    m_text=t;

    update();
}


void LineLabelGraphicsItem::setFont(QFont f){

    if( f==m_font) return;

    prepareGeometryChange();

    m_font=f;

    update();
}

void LineLabelGraphicsItem::setVAlign(Qt::AlignmentFlag a){

    const QSet<Qt::AlignmentFlag> allowed{Qt::AlignBottom, Qt::AlignTop, Qt::AlignVCenter};

    if( a==m_valign ) return;

    prepareGeometryChange();

    Q_ASSERT( allowed.contains(a));

    m_valign=a;

    update();
}

void LineLabelGraphicsItem::setHAlign(Qt::AlignmentFlag a){

    const QSet<Qt::AlignmentFlag> allowed{Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter};

    if( a==m_halign ) return;

    prepareGeometryChange();

    Q_ASSERT( allowed.contains(a));

    m_halign=a;

    update();
}

void LineLabelGraphicsItem::setAlignments( Qt::AlignmentFlag horizontal, Qt::AlignmentFlag vertical ){

    prepareGeometryChange();
    m_halign=horizontal;
    m_valign=vertical;
    update();
}

void LineLabelGraphicsItem::setPadX(qreal px){

    prepareGeometryChange();
    m_padX=px;
    update();
}

void LineLabelGraphicsItem::setPadY(qreal py){

    prepareGeometryChange();
    m_padY=py;
    update();
}

void LineLabelGraphicsItem::setPadding(qreal px, qreal py){

    prepareGeometryChange();
    m_padX=px;
    m_padY=py;
    update();
}

namespace  {

std::ostream& operator<<(std::ostream& os, const QRectF rect){

    os<<"QRectF( x="<<rect.x()<<", y="<<rect.y()<<", w="<<rect.width()<<", h="<<rect.height()<<" )";

    return os;
}

}

QRectF LineLabelGraphicsItem::boundingRect()const{

    // graphicsview framework uses pixels rather than points
    // in order to work on different devices fonts must be specified in pixels

    QRectF trect=QFontMetrics(m_font).boundingRect(m_text);

    qreal tx=trect.x();
    qreal ty=trect.y();
    qreal tw=trect.width();
    qreal th=trect.height();

    switch( m_halign){
    case Qt::AlignLeft: tx+=-tw - m_padX; break;
    case Qt::AlignHCenter: tx+=(m_line.length()-tw)/2; break;
    case Qt::AlignRight: tx+=m_line.length() + m_padX; break;
    default:
        qFatal("Invalid horizontal alignment!!!");
    }

    switch( m_valign ){
    case Qt::AlignTop: ty=-th-m_padY; break;
    case Qt::AlignVCenter: ty+=(th+ty)/2; break;
    case Qt::AlignBottom: ty+=m_padY+th/2; break;
    default:
        qFatal("Invalid vertical alignment!!!");
    }

    return QRectF( tx, ty, tw, th );

}

void LineLabelGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget){

    painter->save();

    QFont font=m_font;

    if( !widget ){

        qreal fac=96./painter->device()->physicalDpiY();    // default screen res is 96 dpi

        font.setPointSizeF(font.pointSizeF()*fac);
    }

    painter->setFont(font);
    painter->setPen(Qt::black);

    QRectF trect=boundingRect();//( tx, ty, tw, th );

    //painter->setBrush(Qt::yellow);
    //painter->drawRect(boundingRect());

    //QRect* actual;
    painter->drawText( trect.bottomLeft(), m_text);


    painter->restore();
}

