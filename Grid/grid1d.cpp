#include "grid1d.h"

#include<limits>


template<>
const Grid1D<float>::value_type Grid1D<float>::NULL_VALUE = std::numeric_limits<Grid1D::value_type>::max();


template<>
const Grid1D<double>::value_type Grid1D<double>::NULL_VALUE = std::numeric_limits<Grid1D::value_type>::max();
