#ifndef PROJECTGEOMETRY_H
#define PROJECTGEOMETRY_H

#include<array>
#include<QPoint>
#include<QPointF>
#include<QTransform>

#include<axxisorientation.h>


class ProjectGeometry{

public:

    static const int NPOINTS=3;

    QPointF coords( int i )const{
        Q_ASSERT( i>=0 && i<NPOINTS);
        return m_xy[i];
    }

    QPointF& coords( int i ){
        Q_ASSERT( i>=0 && i<NPOINTS);
        return m_xy[i];
    }

    QPoint lines( int i )const{
        Q_ASSERT( i>=0 && i<NPOINTS);
        return m_ilxl[i];
    }

    QPoint& lines( int i ){
        Q_ASSERT( i>=0 && i<NPOINTS);
        return m_ilxl[i];
    }

    std::pair<QPoint, QPoint>  linesRange()const;
    QRect bboxLines()const;

    std::pair<QPointF, QPointF>  coordsRange()const;
    QRectF bboxCoords()const;


    bool computeTransforms(QTransform &transformXYToIlXl, QTransform &transformIlXlToXY)const;

    void computeAxxisOrientations( AxxisOrientation& inlineOrientation,
                                   AxxisDirection& inlineDirection, AxxisDirection& crosslineDirection)const;

    double azimuth()const;

private:
    std::array<QPointF, NPOINTS> m_xy;
    std::array<QPoint, NPOINTS>  m_ilxl;  // x==il, y==xl
};


bool isValid( const ProjectGeometry& );

#endif // PROJECTGEOMETRY_H
