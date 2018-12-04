#include "areaitem.h"

#include<cmath>

#include<QColor>
#include<QPen>
#include<QFont>
#include<alignedtextgraphicsitem.h>

AreaItem::AreaItem(AVOProject* project, QGraphicsItem* parentGraphicsItemItem, QObject* parentQObject )
 : QObject( parentQObject), QGraphicsItemGroup(parentGraphicsItemItem), m_project(project)
{
    m_outline=new QGraphicsRectItem(this);
    QPen pen(Qt::blue,2);
    pen.setCosmetic(true);
    m_outline->setPen(pen);
    m_outline->setBrush(Qt::NoBrush);

    m_fill=new QGraphicsRectItem(this);
    QBrush brush(Qt::blue);
    m_fill->setBrush(brush);
    m_fill->setPen(Qt::NoPen);
    m_fill->setOpacity(0.25);
}

QColor AreaItem::outlineColor(){
    return m_outline->pen().color();
}

QColor AreaItem::fillColor(){
    return m_fill->brush().color();
}

qreal AreaItem::fillOpacity(){
    return m_fill->opacity();
}

void AreaItem::setProject(AVOProject* p){

    if( p==m_project ) return;

    m_project=p;

    updateGeometry();
}

void AreaItem::setArea(QRect r){

    if( r==m_area ) return;

    m_area=r;

    updateGeometry();
    updateFrame();
    updateLabels();
}

void AreaItem::setOutlineColor(QColor c){
    auto pen=m_outline->pen();
    pen.setColor(c);
    m_outline->setPen(pen);
}

void AreaItem::setFillColor(QColor c){
    auto brush=m_fill->brush();
    brush.setColor(c);
    m_fill->setBrush(brush);
}

void AreaItem::setFillOpacity(qreal o){
    m_fill->setOpacity(o);
}

void AreaItem::setShowLines(bool on){
    if(on==m_showLines) return;
    m_showLines=on;
    updateLabels();
}

void AreaItem::updateGeometry(){

    if( !m_project ) return;

    auto ilmin=m_area.left();
    //auto ilmax=m_area.right();
    auto nil=m_area.width();

    auto xlmin=m_area.top();
    //auto xlmax=m_area.bottom();
    auto nxl=m_area.height();

    QTransform tf;
    tf.translate( ilmin, xlmin);

    tf.scale(qreal(nil-1)/nil,
            qreal(nxl-1)/nxl );

    QTransform ilxl_to_xy, xy_to_ilxl;
    m_project->geometry().computeTransforms( xy_to_ilxl, ilxl_to_xy );
    tf=tf*ilxl_to_xy;

    setTransform(tf);
}

void AreaItem::updateFrame(){

    QRect r( 0, 0, m_area.width(), m_area.height());
    m_outline->setRect(r);
    m_fill->setRect(r);
}


void AreaItem::updateLabels(){

    QFont font("Times",10,QFont::Normal,true );	// italic
    QPen pen(Qt::darkBlue,1);
    pen.setCosmetic(true);
    QBrush brush(Qt::darkBlue);

    // delete old labels
    for( auto p : m_labelItems ){
        delete p;
    }
    m_labelItems.clear();

    if(!m_showLines) return;

    auto minil=m_area.left();
    auto maxil=m_area.right();
    auto minxl=m_area.top();
    auto maxxl=m_area.bottom();
    auto nil=m_area.width();
    auto nxl=m_area.height();
    auto az=(m_project)?m_project->geometry().azimuth() : 0.;
    /* perpendicular
    for( auto il : {minil,maxil}){

        QString text=QString(" il%1 ").arg(il);
        auto ix=il-minil;

        // left label
        auto il1=new AlignedTextGraphicsItem(text, Qt::AlignRight|Qt::AlignVCenter,az,this);
        il1->setPos(ix, 0);
        il1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        il1->setFont(font);
        il1->setPen(pen);
        m_labelItems.push_back(il1);

        // right label
        auto il2=new AlignedTextGraphicsItem( text, Qt::AlignLeft|Qt::AlignVCenter,az,this);
        il2->setPos(ix, nxl);
        il2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        il2->setFont(font);
        il2->setPen(pen);
        m_labelItems.push_back(il2);
    }

    for( auto xl : {minxl,maxxl}){

        QString text=QString(" xl%1 ").arg(xl);
        auto iy=xl-minxl;

        // bottom label
        auto xl1=new AlignedTextGraphicsItem(text, Qt::AlignVCenter|Qt::AlignLeft,az+90,this);
        xl1->setPos(0,iy);
        xl1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        xl1->setFont(font);
        xl1->setPen(pen);
        m_labelItems.push_back(xl1);

        // top label
        auto xl2=new AlignedTextGraphicsItem(text, Qt::AlignVCenter|Qt::AlignRight,az+90,this);
        xl2->setPos(nil,iy);
        xl2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        xl2->setFont(font);
        xl2->setPen(pen);
        m_labelItems.push_back(xl2);
    }
    */
    // parallel
    // minil
    auto il1a=new AlignedTextGraphicsItem( QString("il %1").arg(minil),
                    Qt::AlignRight|Qt::AlignBottom,az+180,this);
    il1a->setPos(0, 0);
    il1a->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il1a->setFont(font);
    m_labelItems.push_back(il1a);
    auto il1b=new AlignedTextGraphicsItem( QString("il %1").arg(minil),
                    Qt::AlignLeft|Qt::AlignBottom,az+180,this);
    il1b->setPos(0, nxl);
    il1b->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il1b->setFont(font);
    m_labelItems.push_back(il1b);

    // maxil
    auto il2a=new AlignedTextGraphicsItem( QString("il %1").arg(maxil),
                     Qt::AlignLeft|Qt::AlignBottom,az,this);
    il2a->setPos(nil, 0);
    il2a->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il2a->setFont(font);
    m_labelItems.push_back(il2a);
    auto il2b=new AlignedTextGraphicsItem( QString("il %1").arg(maxil),
                     Qt::AlignRight|Qt::AlignBottom,az,this);
    il2b->setPos(nil, nxl);
    il2b->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il2b->setFont(font);
    m_labelItems.push_back(il2b);

    // minxl
    auto xl1a=new AlignedTextGraphicsItem( QString("xl %1").arg(minxl),
                     Qt::AlignLeft|Qt::AlignBottom,az-90,this);
    xl1a->setPos(0, 0);
    xl1a->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl1a->setFont(font);
    m_labelItems.push_back(xl1a);
    auto xl1b=new AlignedTextGraphicsItem( QString("xl %1").arg(minxl),
                     Qt::AlignRight|Qt::AlignBottom,az-90,this);
    xl1b->setPos(nil, 0);
    xl1b->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl1b->setFont(font);
    m_labelItems.push_back(xl1b);

    // maxxl
    auto xl2a=new AlignedTextGraphicsItem( QString("xl %1").arg(maxxl),
                  Qt::AlignRight|Qt::AlignBottom,az+90,this);
    xl2a->setPos(0, nxl);
    xl2a->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl2a->setFont(font);
    m_labelItems.push_back(xl2a);
    auto xl2b=new AlignedTextGraphicsItem( QString("xl %1").arg(maxxl),
                  Qt::AlignLeft|Qt::AlignBottom,az+90,this);
    xl2b->setPos(nil, nxl);
    xl2b->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl2b->setFont(font);
    m_labelItems.push_back(xl2b);

}
