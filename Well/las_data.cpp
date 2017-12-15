#include "las_data.h"

namespace well{

size_t LASData::curveCount()const{
    return curves.size();
}

const std::vector<double>& LASData::curveData( size_t i )const{
    return curves[i];
}

LASMnemonic LASData::curveInfo( size_t i )const{
    return curveInformation[i];
}

}
