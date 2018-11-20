#include "areaitem.h"

#include<cmath>

#include<QColor>
#include<QPen>
#include<QFont>
#include<alignedtextgraphicsitem.h>

AreaItem::AreaItem(AVOProject* project, QGraphicsItem* parentGraphicsItemItem, QObject* parentQObject )
 : QObject( parentQObject), QGraphicsRectItem(parentGraphicsItemItem), m_project(project)
{
    QBrush brush(Qt::white);
    setBrush(brush);
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


void AreaItem::updateGeometry(){

    if( !m_project ) return;

    auto ilmin=m_area.left();
    auto ilmax=m_area.right();
    auto nil=m_area.width();

    auto xlmin=m_area.top();
    auto xlmax=m_area.bottom();
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
    setRect(r);
    QPen framePen( Qt::darkBlue,2);
    framePen.setCosmetic(true);
    setPen(framePen);
}


void AreaItem::updateLabels(){

    QFont font("Times",14,QFont::Bold);
    QPen pen(Qt::darkBlue,1);
    pen.setCosmetic(true);
    QBrush brush(Qt::darkBlue);

    // delete old labels
    for( auto p : m_labelItems ){
        delete p;
    }
    m_labelItems.clear();

    auto minil=m_area.left();
    auto maxil=m_area.right();
    auto minxl=m_area.top();
    auto maxxl=m_area.bottom();
    auto nil=m_area.width();
    auto nxl=m_area.height();
    auto az=(m_project)?m_project->geometry().azimuth() : 0.;

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
   /*
    // minil
    auto il1=new AlignedTextGraphicsItem( QString("il %1").arg(minil),
                    Qt::AlignHCenter|Qt::AlignBottom,az+180,this);
    il1->setPos(0, nxl/2);
    il1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il1->setFont(font);
    m_labelItems.push_back(il1);

    // maxil
    auto il2=new AlignedTextGraphicsItem( QString("il %1").arg(maxil),
                     Qt::AlignHCenter|Qt::AlignBottom,az,this);
    il2->setPos(nil, nxl/2);
    il2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    il2->setFont(font);
    m_labelItems.push_back(il2);

    // minxl
    auto xl1=new AlignedTextGraphicsItem( QString("xl %1").arg(minxl),
                     Qt::AlignHCenter|Qt::AlignBottom,az-90,this);
    xl1->setPos(nil/2, 0);
    xl1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl1->setFont(font);
    m_labelItems.push_back(xl1);

    // maxxl
    auto xl2=new AlignedTextGraphicsItem( QString("xl %1").arg(maxxl),
                  Qt::AlignHCenter|Qt::AlignBottom,az+90,this);
    xl2->setPos(nil/2, nxl);
    xl2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    xl2->setFont(font);
    m_labelItems.push_back(xl2);
    */
}
