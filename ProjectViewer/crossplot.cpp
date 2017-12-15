#include "crossplot.h"

#include<algorithm>
#include<random>
#include<numeric>
#include<iostream>

namespace crossplot{



Data createFromGrids( Grid2D<float>* grid1, Grid2D<float>* grid2, Grid2D<float>* horizon, Grid2D<float>* grida ){

    Data data;

    Grid2DBounds bounds( std::min( grid1->bounds().i1(), grid2->bounds().i1()),
                       std::min( grid1->bounds().j1(), grid2->bounds().j1()),
                       std::max( grid1->bounds().i2(), grid2->bounds().i2()),
                       std::max( grid1->bounds().j2(), grid2->bounds().j2()) );

    // add check for matching grid extents

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            double v1=(*grid1)(i,j);
            if( v1==grid1->NULL_VALUE) continue;

            double v2=(*grid2)(i,j);
            if( v2==grid2->NULL_VALUE) continue;

            double t=0;
            if( horizon ){

                t=(*horizon)(i,j); // do not handle null values
                t*=0.001;           // convert time from millis to seconds
            }

            double a=0;
            if( grida ){

                a=(*grida)(i,j);
            }

            data.push_back(DataPoint( v1, v2, i, j, t, a) );
        }
    }


    return data;
}


Data createFromVolumes( Grid3D<float>* volume1, Grid3D<float>* volume2,
                        Grid3D<float>* volumea){

    Data data;

    Grid2DBounds bounds(volume1->bounds().i1(), volume1->bounds().j1(),
                        volume1->bounds().i2(), volume1->bounds().j2());
    int nt=volume1->bounds().nt();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            for( int k=0; k<=nt; k++){
                double v1=(*volume1)(i,j,k);
                double v2=(*volume2)(i,j,k);
                if( v1==volume1->NULL_VALUE || v2==volume2->NULL_VALUE) continue;

                double a=(volumea) ? (*volumea)(i,j,k) : 0;

                data.push_back(DataPoint(
                               v1, v2, i, j, volume1->bounds().sampleToTime(k), a) );
            }

        }

    }

    return data;
}

Data createFromVolumesReduced( Grid3D<float>* volume1, Grid3D<float>* volume2,
                               size_t usedPoints, Grid3D<float>* volumea ){

    Data data;

    Grid2DBounds bounds(volume1->bounds().i1(), volume1->bounds().j1(),
                        volume1->bounds().i2(), volume1->bounds().j2());
    int nt=volume1->bounds().nt();

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

        double a=(volumea) ? (*volumea)[idx] : 0;

        int k=idx%nt;
        idx/=nt;
        int j=bounds.j1() + idx%bounds.width();
        idx/=bounds.width();
        int i=bounds.i1() + idx;


        data.push_back(DataPoint(
                           v1, v2, i, j, volume1->bounds().sampleToTime(k), a) );
    }

    return data;
}


Data createFromLogs( Log* log1, Log* log2, int iline, int xline ){

    Data data;

    if( !log1 ) return data;
    if( !log2 ) return data;
    if( log1->nz() != log2->nz() ) return data;

    for( int i=0; i<log1->nz(); i++){

        auto z = 0.001 * ( log1->z0() + i * log1->dz() );

        double v1=(*log1)[i];
        double v2=(*log2)[i];
        if( v1==log1->NULL_VALUE || v2==log2->NULL_VALUE) continue;

        data.push_back( DataPoint( v1, v2, iline, xline, z, 0));
    }
    return data;
}


Data createFromLogs( Log* log1, Log* log2, Log* loga, int iline, int xline ){

    Data data;

    if( !loga ) return createFromLogs( log1, log2, iline, xline );
    if( !log1 ) return data;
    if( !log2 ) return data;

    if( log1->nz() != log2->nz() ) return data;

    for( int i=0; i<log1->nz(); i++){

        auto z = 0.001 * ( log1->z0() + i * log1->dz() );

        double v1=(*log1)[i];
        double v2=(*log2)[i];
        double va=(*loga)[i];
        if( v1==log1->NULL_VALUE || v2==log2->NULL_VALUE || va==loga->NULL_VALUE) continue;

        data.push_back( DataPoint( v1, v2, iline, xline, z, va));
    }
    return data;
}

}
