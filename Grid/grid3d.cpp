#include "grid3d.h"

#include<limits>

template<>
const Grid3D<float>::value_type Grid3D<float>::NULL_VALUE = std::numeric_limits<Grid3D::value_type>::max();


template<>
const Grid3D<double>::value_type Grid3D<double>::NULL_VALUE = std::numeric_limits<Grid3D::value_type>::max();


