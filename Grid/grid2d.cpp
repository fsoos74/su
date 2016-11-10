#include "grid2d.h"

#include<limits>

#include<QPointF>

namespace seismic {
    class Trace;
}

template<>
const Grid2D<float>::value_type Grid2D<float>::NULL_VALUE = std::numeric_limits<Grid2D::value_type>::max();


template<>
const Grid2D<std::pair<double, double>>::value_type Grid2D<std::pair<double, double>>::NULL_VALUE =
        std::pair<double, double>(std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );

template<>
const Grid2D<QPointF>::value_type Grid2D<QPointF>::NULL_VALUE =
        QPointF(std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max() );


template<>
const Grid2D<seismic::Trace*>::value_type Grid2D<seismic::Trace*>::NULL_VALUE=nullptr;


std::pair<float, float> valueRange( const Grid2D<float>& grid){

    float min=std::numeric_limits<float>::max();
    float max=std::numeric_limits<float>::lowest();

    for( Grid2D<float>::values_type::const_iterator it=grid.values().cbegin(); it!=grid.values().cend(); ++it){

        if(*it==grid.NULL_VALUE) continue;

        if( *it<min ){
            min=*it;
        }
        if( *it>max ){
            max=*it;
        }
    }

    if( max<min){
        max=grid.NULL_VALUE;
        min=grid.NULL_VALUE;
    }

    return std::pair<float, float>(min,max);
}

