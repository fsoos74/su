#include "log.h"

#include<limits>

double Log::NULL_VALUE = std::numeric_limits<double>::max();

void Log::setName(const std::string & name){
    m_name=name;
}

void Log::setUnit(const std::string & unit){
    m_unit=unit;
}

void Log::setDescr(const std::string & descr){
    m_descr=descr;
}

void Log::setZ0(double z){
    m_z0=z;
}

void Log::setDZ(double dz){
    m_dz=dz;
}

std::pair<double, double> Log::range()const{


    /* cannot use std because of null value
    auto minmax=std::minmax_element(m_log->cbegin(), m_log->cend());
    */

    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    for( auto v : m_data){
        if( v==NULL_VALUE) continue;
        if( v<min ) min=v;
        if( v>max ) max=v;
    }

    return std::make_pair(min, max);
}
