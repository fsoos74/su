#include "trackrulerview.h"

#include<QGraphicsPathItem>
#include<cmath>
#include<iostream>


TrackRulerView::TrackRulerView(QWidget* parent):RulerAxisView(parent)
{

}

void TrackRulerView::setLog(std::shared_ptr<Log> l){

    if( l == m_log ) return;

    m_log=l;

    auto range=m_log->range();
    xAxis()->setRange(range.first, range.second);
    xAxis()->setViewRange(range.first, range.second);
    zAxis()->setRange(l->z0(), l->lz());
    zAxis()->setViewRange(l->z0(), l->lz());

    refreshScene();

    //fitInView(sceneRect(), Qt::IgnoreAspectRatio);
}


void TrackRulerView::setHighlighted(QVector<qreal> h){

    m_highlighted=h;
    refreshScene();
}

QPainterPath TrackRulerView::buildPath(const Log& log){

    QPainterPath path;

    bool isDown=false;

    for( auto i=0; i<log.nz(); i++){

        auto x = log[i];
        if( x==log.NULL_VALUE){
            isDown=false;
            continue;
        }
        auto z = log.index2z(i);

        x=xAxis()->toScene(x);
        z=zAxis()->toScene(z);

        QPointF p(x,z);
        if( isDown ){
            path.lineTo(p);
        }
        else{
            path.moveTo(p);
            isDown=true;
        }

    }

    return path;
}


void TrackRulerView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    if( m_log ){

        // draw log
        auto path=buildPath(*m_log);
        QGraphicsPathItem* gitem=new QGraphicsPathItem(path);
        gitem->setPen( QPen( Qt::darkBlue, 0));
        scene->addItem( gitem );

        // draw highlighted, do this in scene because drawforeground did not work as expected
        for( auto zh : m_highlighted){
            qreal z=zAxis()->toScene(zh);
            qreal zh2 = zh + m_log->dz();
            qreal z2=zAxis()->toScene(zh2);

            QGraphicsRectItem* item=new QGraphicsRectItem(sceneRect().left(), z, sceneRect().width() , z2-z);
            item->setPen( Qt::NoPen );
            item->setBrush( QBrush(qRgba(255,0,0,64) ) );
            item->setOpacity(0.5);
            scene->addItem(item);
        }

    }

    setScene(scene);
}
