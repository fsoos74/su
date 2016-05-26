#include "crossplot.h"

namespace crossplot{



Data createFromGrids( Grid2D<double>* grid1, Grid2D<double>* grid2 ){

    Data data;

    Grid2DBounds bounds( std::min( grid1->bounds().i1(), grid2->bounds().i1()),
                       std::min( grid1->bounds().j1(), grid2->bounds().j1()),
                       std::max( grid1->bounds().i2(), grid2->bounds().i2()),
                       std::max( grid1->bounds().j2(), grid2->bounds().j2()) );

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            double v1=(*grid1)(i,j);
            if( v1==grid1->NULL_VALUE) continue;

            double v2=(*grid2)(i,j);
            if( v2==grid2->NULL_VALUE) continue;

            data.push_back(DataPoint( v1, v2, i, j, 0) );

        }
    }

    return data;
}


Data createFromVolumes( Grid3D<float>* volume1, Grid3D<float>* volume2 ){

    Data data;

    Grid2DBounds bounds(volume1->bounds().inline1(), volume1->bounds().crossline1(),
                        volume1->bounds().inline2(), volume1->bounds().crossline2());
    int nt=volume1->bounds().sampleCount();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            for( int k=0; k<=nt; k++){
                double v1=(*volume1)(i,j,k);
                double v2=(*volume2)(i,j,k);
                if( v1==volume1->NULL_VALUE || v2==volume2->NULL_VALUE) continue;
                data.push_back(DataPoint(
                               v1, v2, i, j, volume1->bounds().sampleToTime(k)) );
            }

        }

    }

    return data;
}

Data createFromVolumesReduced( Grid3D<float>* volume1, Grid3D<float>* volume2,
                               size_t usedPoints ){

    Data data;

    Grid2DBounds bounds(volume1->bounds().inline1(), volume1->bounds().crossline1(),
                        volume1->bounds().inline2(), volume1->bounds().crossline2());
    int nt=volume1->bounds().sampleCount();

    size_t numInputPoints=volume1->size();

    // create random list of volume indices with NO DUPLICATES
    //std::cout<<"Populating crossplot with random points"<<std::endl<<std::flush;
    std::vector<size_t> indices(numInputPoints);
    std::iota( indices.begin(), indices.end(), 0);
    std::mt19937 eng{std::random_device{}()};
    std::shuffle(indices.begin(), indices.end(), eng);

    // add crossplot data points for the first MAX_POINTS indices

    for( int n=0; n<usedPoints; n++){
        // convert index to inline, crossline and sample
        size_t idx=indices[n];

        double v1=(*volume1)[idx];
        double v2=(*volume2)[idx];
        if( v1==volume1->NULL_VALUE || v2==volume2->NULL_VALUE) continue;

        int k=idx%nt;
        idx/=nt;
        int j=bounds.j1() + idx%bounds.width();
        idx/=bounds.width();
        int i=bounds.i1() + idx;

        data.push_back(DataPoint(
                           v1, v2, i, j, volume1->bounds().sampleToTime(k)) );
    }

    return data;
}




}
