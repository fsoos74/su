#ifndef CROSSPLOT_H
#define CROSSPLOT_H

#include<vector>

#include "grid2d.h"
#include "grid3d.h"
#include "log.h"
#include <wellpath.h>

#include<QTransform>

namespace crossplot{


struct DataPoint{
    DataPoint():x(0),y(0), iline(0),xline(0), time(0), attribute(0){}
    DataPoint( float ix, float iy, int iiline, int ixline, float(itime), float iattribute=0):
        x(ix), y(iy), iline(iiline), xline(ixline), time(itime), attribute(iattribute){}

    float x=0;
    float y=0;

    int iline=0;
    int xline=0;
    float time=0;
    float attribute=0;
};


typedef std::vector<DataPoint> Data;

Data createFromGrids( Grid2D<float>* grid_x, Grid2D<float>* grid_y,
                      Grid2D<float>* horizon=nullptr, Grid2D<float>* grid_attr=nullptr );

Data createFromVolumes( Grid3D<float>* volume_x, Grid3D<float>* volume_y,
                        Grid3D<float>* volume_attr=nullptr );

// used only n random selected points (inline,xline,sample number)
Data createFromVolumesReduced( Grid3D<float>* volume_x, Grid3D<float>* volume_y,
                               size_t n, Grid3D<float>* volume_attr=nullptr );

Data createFromLogs( Log* log1, Log* log2, int iline=1, int xline=1 );  // uses md

Data createFromLogs( Log* log1, Log* log2, Log* loga, int iline=1, int xline=1 );   // uses md

Data createFromLogs( Log* log1, Log* log2, Log* loga, const WellPath& wp, QTransform xy_to_ilxl);      // points have varying il, xl, and z according to wellpath

}


#endif // CROSSPLOT_H
