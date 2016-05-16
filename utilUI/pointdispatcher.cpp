#include "pointdispatcher.h"

#include "baseviewer.h"

PointDispatcher::PointDispatcher()
{

}


void PointDispatcher::sendPoint( SelectionPoint point, BaseViewer* sender ){

    for( BaseViewer* viewer : m_viewers){

        if( viewer==sender ) continue;

        if( viewer->receptionEnabled()){

            viewer->receivePoint(point);

        }

    }
}



void PointDispatcher::sendPoints( QVector<SelectionPoint> points, int code, BaseViewer* sender){

    for( BaseViewer* viewer : m_viewers){

        if( viewer==sender ) continue;

        if( viewer->receptionEnabled()){

            viewer->receivePoints(points, code );
        }
    }
}

void PointDispatcher::addViewer( BaseViewer* viewer ){

    if( m_viewers.contains(viewer)) return;     // don't allow duplicates

    m_viewers.insert(viewer);
}

void PointDispatcher::removeViewer( BaseViewer* viewer){

    m_viewers.remove(viewer);
}
