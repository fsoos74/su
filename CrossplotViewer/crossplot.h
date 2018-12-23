#ifndef CROSSPLOT_H
#define CROSSPLOT_H


#include "grid2d.h"
#include "grid3d.h"
#include "log.h"
#include <wellpath.h>

#include<QVector>
#include<QTransform>


class Crossplot{

public:

    struct DataPoint{
        DataPoint():x(0),y(0), iline(0),xline(0), time(0), attribute(0),dataset(0){}
        DataPoint( float ix, float iy, int iiline, int ixline, float(itime), float iattribute=0, int ids=0):
        x(ix), y(iy), iline(iiline), xline(ixline), time(itime), attribute(iattribute), dataset(ids){}

        float x;
        float y;
        int iline;
        int xline;
        float time;
        float attribute;
        int dataset;
    };

    using Data=QVector<DataPoint>;

    inline int size()const{
        return m_data.size();
    }

    inline int datasetCount()const{
        return m_datasetNames.size();
    }

    inline QString datasetName(int i)const{
        return m_datasetNames.at(i);
    }

    const Data& data()const{
        return m_data;
    }

    void addDataset(const Data&, QString name="" );
    void add(const Crossplot& other);

    static Data createFromGrids( Grid2D<float>* grid_x, Grid2D<float>* grid_y,
                      Grid2D<float>* horizon=nullptr, Grid2D<float>* grid_attr=nullptr );

    static Data createFromVolumes( Grid3D<float>* volume_x, Grid3D<float>* volume_y,
                        Grid3D<float>* volume_attr=nullptr );

    // used only n random selected points (inline,xline,sample number)
    static Data createFromVolumesReduced( Grid3D<float>* volume_x, Grid3D<float>* volume_y,
                       size_t n, Grid3D<float>* volume_attr=nullptr );

    static Data createFromLogs( Log* log1, Log* log2, int iline=1, int xline=1 );  // uses md

    static Data createFromLogs( Log* log1, Log* log2, Log* loga, int iline=1, int xline=1 );   // uses md

    // points have varying il, xl, and z according to wellpath
    static Data createFromLogs( Log* log1, Log* log2, Log* loga, const WellPath& wp, QTransform xy_to_ilxl);

private:
    QVector<QString> m_datasetNames;
    Data m_data;
};



#endif // CROSSPLOT_H
