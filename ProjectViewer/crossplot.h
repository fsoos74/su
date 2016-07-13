#ifndef CROSSPLOT_H
#define CROSSPLOT_H

#include<vector>

#include "grid2d.h"
#include "grid3d.h"


namespace crossplot{


struct DataPoint{
    DataPoint():x(0),y(0), iline(0),xline(0), time(0){}
    DataPoint( float ix, float iy, int iiline, int ixline, float(itime)):
        x(ix), y(iy), iline(iiline), xline(ixline), time(itime){}

    float x=0;
    float y=0;

    int iline=0;
    int xline=0;
    float time=0;
};


typedef std::vector<DataPoint> Data;

Data createFromGrids( Grid2D<double>* grid1, Grid2D<double>* grid2 );

Data createFromVolumes( Grid3D<float>* volume1, Grid3D<float>* volume2 );

// used only n random selected points (inline,xline,sample number)
Data createFromVolumesReduced( Grid3D<float>* volume1, Grid3D<float>* volume2, size_t n );


}


#endif // CROSSPLOT_H