#include "areaitem.h"

#include<cmath>
#include<QColor>
#include<QPen>
#include<alignedtextgraphicsitem.h>
#include<alignedtextitem.h>

AreaItem::AreaItem(AVOProject* project, QGraphicsItem* parentGraphicsItemItem, QObject* parentQObject )
 : QObject( parentQObject), QGraphicsRectItem(parentGraphicsItemItem), m_project(project)
{
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
    QPen framePen( Qt::black,2);
    framePen.setCosmetic(true);
    setPen(framePen);
}


void AreaItem::updateLabels(){

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
    // compute azimuth
    auto geom=m_project->geometry();
    auto p1=geom.coords(0);
    auto p2=geom.coords(2);
    auto az=180./M_PI*std::atan2(p2.x()-p1.x(), p2.y()-p1.y());  // degrees
    std::cout<<"azimuth="<<az<<std::endl;


    // minil
    auto il1=new AlignedTextGraphicsItem( QString("il %1").arg(minil),
                    Qt::AlignHCenter|Qt::AlignBottom,az+180,this);
    il1->setPos(0, nxl/2);
    il1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(il1);

    // maxil
    auto il2=new AlignedTextGraphicsItem( QString("il %1").arg(maxil),
                     Qt::AlignHCenter|Qt::AlignBottom,az,this);
    il2->setPos(nil, nxl/2);
    il2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(il2);

    // minxl
    auto xl1=new AlignedTextGraphicsItem( QString("xl %1").arg(minxl),
                     Qt::AlignHCenter|Qt::AlignBottom,az-90,this);
    xl1->setPos(nil/2, 0);
    xl1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(xl1);

    // maxxl
    auto xl2=new AlignedTextGraphicsItem( QString("xl %1").arg(maxxl),
                  Qt::AlignHCenter|Qt::AlignBottom,az+90,this);
    xl2->setPos(nil/2, nxl);
    xl2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(xl2);
}

/*
void AreaItem::updateLabels(){

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

    // minil
    auto il1=new AlignedTextItem( QString("il %1").arg(minil), this);
    il1->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    il1->setPos(0, nxl/2);
    il1->setRotation(-90);
    il1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(il1);

    // maxil
    auto il2=new AlignedTextItem( QString("il %1").arg(maxil), this);
    il2->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    il2->setPos(nil, nxl/2);
    il2->setRotation(90);
    il2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(il2);

    // minxl
    auto xl1=new AlignedTextItem( QString("xl %1").arg(minxl), this);
    xl1->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    xl1->setPos(nil/2, 0);
    xl1->setRotation(0);
    xl1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(xl1);

    // maxxl
    auto xl2=new AlignedTextItem( QString("xl %1").arg(maxxl), this);
    xl2->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    xl2->setPos(nil/2, nxl);
    xl2->setRotation(180);
    xl2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_labelItems.push_back(xl2);
}
*/
