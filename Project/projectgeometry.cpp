#include "projectgeometry.h"

#include<cmath>

#include<iostream>


std::pair<QPoint, QPoint> ProjectGeometry::linesRange()const{

    QPoint min(lines(0));
    QPoint max(lines(0));

    for( int i=1; i<3; i++){

        int il=lines(i).x();
        if( il < min.x() ) min.setX(il);
        if( il > max.x() ) max.setX(il);

        int xl=lines(i).y();
        if( xl < min.y() ) min.setY(xl);
        if( xl > max.y() ) max.setY(xl);
    }

    return std::pair<QPoint, QPoint>( min, max);
}


QRect ProjectGeometry::bboxLines()const{

    auto range=linesRange();

    return QRect( range.first, range.second);
}


std::pair<QPointF, QPointF>  ProjectGeometry::coordsRange()const{

    // compute 4th corner (max il, max xl) : this is p1(min il, min xl) mirrored at line p2-p3
    const QPointF& P1=coords(0);
    const QPointF& P2=coords(1);
    const QPointF& P3=coords(2);

    // line direction vector
    qreal nx=P3.x() - P2.x();
    qreal ny=P3.y() - P2.y();

    // make unit vector
    qreal d=std::sqrt( std::pow(nx, 2) + std::pow(ny,2) );
    nx/=d;
    ny/=d;

    // compute intersection point
    qreal ki=nx*(P1.x() - P2.x()) + ny*(P1.y() - P2.y() );
    qreal xi=P2.x() + ki*nx;
    qreal yi=P2.y() + ki*ny;

    // compute mirrored point
    QPointF P4( 2*xi - P1.x(), 2*yi - P1.y() );

    // find min/max
    QPointF min(P4);
    QPointF max(P4);

    for( int i=0; i<3; i++){

        qreal x=coords(i).x();
        if( x < min.x() ) min.setX(x);
        if( x > max.x() ) max.setX(x);

        qreal y=coords(i).y();
        if( y < min.y() ) min.setY(y);
        if( y > max.y() ) max.setY(y);
    }

    return std::pair<QPointF, QPointF>( min, max);

}

QRectF ProjectGeometry::bboxCoords()const{

    auto range=coordsRange();

    return QRectF( range.first, range.second);
}

bool ProjectGeometry::computeTransforms(QTransform &transformXYToIlXl, QTransform &transformIlXlToXY)const{

    const qreal EPS=0.001;

    int il1=lines(0).x();
    int xl1=lines(0).y();
    int il2=lines(1).x();
    int xl2=lines(1).y();
    int il3=lines(2).x();
    int xl3=lines(2).y();

    qreal x1=coords(0).x();
    qreal y1=coords(0).y();
    qreal x2=coords(1).x();
    qreal y2=coords(1).y();
    qreal x3=coords(2).x();
    qreal y3=coords(2).y();

    int il21=il2-il1;
    int il31=il3-il1;
    int xl21=xl2-xl1;
    int xl31=xl3-xl1;
    qreal x21=x2-x1;
    qreal x31=x3-x1;
    qreal y21=y2-y1;
    qreal y31=y3-y1;

    qreal D=x31*y21 - x21*y31;
    if( std::fabs(D)<EPS ){
        return false;
    }

    qreal m11=( il21*y31 + il31*y21 ) /D;

    if( std::fabs(y21)<EPS && std::fabs(y31)<EPS){
        return false;
    }

    qreal m21=( std::fabs(y21) > std::fabs(y31) ) ?
                ( il21 - m11*x21 ) / y21 :
                ( il31 - m11*x31 ) / y31;

    qreal dx= il1 - m11*x1 - m21*y1;


    D=y21*x31 - y31*x21;
    if( std::fabs(D)<EPS ){
        return false;
    }

    qreal m22=( xl21*x31 - xl31*x21 ) / D;

    if( std::fabs(y21)<EPS && std::fabs(y31)<EPS ){
        return false;
    }

    qreal m12=(std::fabs(x21) > std::fabs(x31) ) ?
                ( xl21 - m22*y21 ) / x21 :
                ( xl31 - m22*y31 ) / x31;

    qreal dy= xl1- m22*y1 - m12*x1;

    transformXYToIlXl.setMatrix( m11, m12, 0, m21, m22, 0, dx, dy, 1);
    bool ok=false;
    transformIlXlToXY=transformXYToIlXl.inverted(&ok);

    return ok;
}


void ProjectGeometry::computeAxxisOrientations( AxxisOrientation& inlineOrientation,
                                               AxxisDirection& inlineDirection, AxxisDirection& crosslineDirection)const{

    int il1=lines(0).x();
    int xl1=lines(0).y();
    int il2=lines(1).x();
    int xl2=lines(1).y();
    int il3=lines(2).x();
    int xl3=lines(2).y();

    Q_ASSERT( il1==il2 );

    qreal x1=coords(0).x();
    qreal y1=coords(0).y();
    qreal x2=coords(1).x();
    qreal y2=coords(1).y();
    qreal x3=coords(2).x();
    qreal y3=coords(2).y();


    if( std::fabs(x2-x1) > std::fabs(y2-y1) ){  // inlines horizontal, crosslines vertical
        inlineOrientation=AxxisOrientation::Horizontal;
        inlineDirection= ( y3 > y1 ) ? AxxisDirection::Descending : AxxisDirection::Ascending;          // y origin at bottom
        crosslineDirection= ( x2 > x1 ) ? AxxisDirection::Ascending : AxxisDirection::Descending;
    }
    else{   // inlines vertical, crosslines horizontal
        inlineOrientation=AxxisOrientation::Vertical;
        inlineDirection= ( x3 > x1 ) ? AxxisDirection::Ascending : AxxisDirection::Descending;
        crosslineDirection= ( y2 > y1 ) ? AxxisDirection::Descending : AxxisDirection::Ascending;       // y origin at bottom
    }
}


bool isValid( const ProjectGeometry& geom){

    QTransform transformXYToIlXl;
    QTransform transformIlXlToXY;

    return geom.computeTransforms(transformXYToIlXl, transformIlXlToXY);
}


double ProjectGeometry::azimuth()const{
    //define own because of msvc issues
    #define PI 3.141592654
    auto p1=coords(0);
    auto p2=coords(2);
    auto az=180./PI*std::atan2(p2.x()-p1.x(), p2.y()-p1.y());  // degrees
    return az;
}


