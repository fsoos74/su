#include "projectgeometry.h"

#include<QTransform>
#include<QPoint>
#include<QPointF>
#include<cmath>

ProjectGeometry::ProjectGeometry()
{

}

void ProjectGeometry::setInlineAndCrossline( int i, QPoint inlineAndCrossline){

    Q_ASSERT( i>=0 && i<N_POINTS);

    m_inlineAndCrossline[i]=inlineAndCrossline;
}

void ProjectGeometry::setCoordinates( int i, QPointF p){

    Q_ASSERT( i>=0 && i<N_POINTS);

    m_coords[i]=p;
}


bool isApprox(const qreal& x1, const qreal& x2, const qreal& EPS=0.0001){

    return std::fabs(x2-x1)<EPS;
}

// points are defined as:
// point1 (0): origin
// point2 (1): first inline, last crossline
// point3 (2): first crossline, last inline
bool isValid( const ProjectGeometry& geom){

    qreal EPS=0.00001;

    if(geom.inlineAndCrossline(0).x()!=geom.inlineAndCrossline(1).x() ||
            geom.inlineAndCrossline(0).y() >= geom.inlineAndCrossline(1).y() ) return false;
    if(geom.inlineAndCrossline(0).x()>=geom.inlineAndCrossline(2).x() ||
            geom.inlineAndCrossline(0).y() != geom.inlineAndCrossline(2).y() ) return false;

    if(isApprox(geom.coordinates(0).x(), geom.coordinates(1).x()) &&
            isApprox(geom.coordinates(0).x(), geom.coordinates(2).x())) return false;

    if(isApprox(geom.coordinates(0).y(), geom.coordinates(1).y()) &&
            isApprox(geom.coordinates(0).y(), geom.coordinates(2).y())) return false;


    return true;
}


bool ProjectGeometry::computeTransforms(QTransform &transformXYToIlXl, QTransform &transformIlXlToXY)const{

    const qreal EPS=0.001;

    QPoint inlineAndCrossline1=inlineAndCrossline(0);
    QPointF coords1=coordinates(0);
    QPoint inlineAndCrossline2=inlineAndCrossline(1);
    QPointF coords2=coordinates(1);
    QPoint inlineAndCrossline3=inlineAndCrossline(2);
    QPointF coords3=coordinates(2);

    int il1=inlineAndCrossline1.x();
    int xl1=inlineAndCrossline1.y();
    int il2=inlineAndCrossline2.x();
    int xl2=inlineAndCrossline2.y();
    int il3=inlineAndCrossline3.x();
    int xl3=inlineAndCrossline3.y();

    int x1=coords1.x();
    int y1=coords1.y();
    int x2=coords2.x();
    int y2=coords2.y();
    int x3=coords3.x();
    int y3=coords3.y();

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
