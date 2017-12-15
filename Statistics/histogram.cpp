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
if( std::isnan(v)) return;
    // statistics are computed using Welford's algorithm
    m_n++;

    // add value to bins
    size_t i=(v-m_firstBin)/m_binWidth;
    if( v<m_firstBin ){
        m_underflowCount++;
    }
    else if(i>=m_binCount){
        m_overflowCount++;
    }
    else{
        m_bins[i]++;
    }

    // update range
    if( v<m_min ) m_min=v;
    if( v>m_max ) m_max=v;

    // update statistics
    if( m_n==1){
        m_oldM = m_newM = v;
        m_oldS = 0.0;
    }
    else{
        m_newM = m_oldM + (v - m_oldM)/m_n;
        m_newS = m_oldS + (v - m_oldM)*(v - m_newM);
        m_oldM = m_newM;
        m_oldS = m_newS;
    }
}
