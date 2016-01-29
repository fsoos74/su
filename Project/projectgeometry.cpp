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



