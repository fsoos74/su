#ifndef PROJECTGEOMETRY_H
#define PROJECTGEOMETRY_H

#include<array>
#include<QPoint>
#include<QPointF>
#include<QTransform>
#include<axxisorientation.h>

// QPoint used for inline(x) and crossline(y)

class ProjectGeometry
{
public:

    ProjectGeometry();

    QPoint inlineAndCrossline( int i )const{
        Q_ASSERT(i>=0 && i<N_POINTS);
        return m_inlineAndCrossline[i];
    }

    QPointF coordinates(int i)const{
        Q_ASSERT(i>=0 && i<N_POINTS);
        return m_coords[i];
    }

    void setInlineAndCrossline( int, QPoint);
    void setCoordinates( int, QPointF);

    bool computeTransforms(QTransform &transformXYToIlXl, QTransform &transformIlXlToXY)const;

    void computeAxxisOrientations( AxxisOrientation& inlineOrientation, AxxisDirection& inlineDirection, AxxisDirection& CrosslineDirection);

    double inlineAzimuth()const;

private:

    static const int N_POINTS=3;
    std::array<QPoint, N_POINTS > m_inlineAndCrossline;
    std::array<QPointF, N_POINTS> m_coords;
};

bool isValid( const ProjectGeometry&);


#endif // PROJECTGEOMETRY_H
