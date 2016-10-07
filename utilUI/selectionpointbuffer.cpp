#include "selectionpointbuffer.h"

#include<QString>
#include<QSet>

SelectionPointBuffer::SelectionPointBuffer(QObject *parent) : QObject(parent)
{

}


void SelectionPointBuffer::setPoints( BaseViewer* v, QVector<SelectionPoint> p ){

    //omit check for equal because speed
    //if( points(v) == p ) return;

    // remove v from list if it has no points
    if( p.empty() ){
        m_points.remove( v );
    }
    else{
        m_points.insert( v, p );
    }
}


QVector<SelectionPoint> SelectionPointBuffer::intersectionPoints( QVector<SelectionPoint> ref){

    QSet<QString> refSet;

    // need to test if it is faster to usesp directly instead of qstring
    for( auto p : ref ){
        QString key=QString("%1_%2").arg(p.iline, p.xline);
        refSet.insert(key);
    }

    QVector<SelectionPoint> inters;

    for( BaseViewer* v : m_points.keys() ){

        auto viewerPoints = points(v);

        for( SelectionPoint p : viewerPoints ){

            QString key=QString("%1_%2").arg(p.iline, p.xline);

            if( refSet.contains( key ) ){
                inters.append( p );
            }
        }

    }


    return inters;
}
