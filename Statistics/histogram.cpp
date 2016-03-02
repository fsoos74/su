#include "histogram.h"

#include <stdexcept>

Histogram::Histogram( const double& firstBin, const double& binIncr, size_t numberOfBins)
    :m_firstBin(firstBin), m_binWidth(binIncr), m_binCount(numberOfBins),m_bins(numberOfBins,0){

    if( m_binWidth<=0){
        throw std::runtime_error("invalid bin width");
    }

}


// bounds of bin [min,max), throws error for out of bounds i
std::pair<double,double> Histogram::binInterval(const size_t i)const{

    if( i>=m_binCount){
       throw std::out_of_range("invalid bin index");
    }

    return std::pair<double, double>(m_firstBin+i*m_binWidth, m_firstBin+(i+1)*m_binWidth);
}

// throws for out of bounds index
size_t Histogram::binValue(const size_t i)const{

    return m_bins.at(i);
}

void Histogram::addValue( const double& v){

    m_totalCount++;

    if( v<m_firstBin ){
        m_underflowCount++;
        return;
    }

    size_t i=(v-m_firstBin)/m_binWidth;

    if(i>=m_binCount){
        m_overflowCount++;
        return;
    }

    m_bins[i]++;
}
