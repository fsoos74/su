#ifndef HISTOGRAM_H
#define HISTOGRAM_H


#include<vector>
#include<cstddef>   //size_t

class Histogram
{

public:

    Histogram( const double& firstBin=0, const double& binIncr=1, size_t numberOfBins=1);

    double firstBin()const{
        return m_firstBin;
    }

    double binWidth()const{
        return m_binWidth;
    }

    size_t binCount()const{
        return m_binCount;
    }

    size_t underflowCount()const{
        return m_underflowCount;
    }

    size_t overflowCount()const{
        return m_overflowCount;
    }

    size_t totalCount()const{
        return m_totalCount;
    }

    // bounds of bin [min,max), throws error for out of bounds i
    std::pair<double,double> binInterval(const size_t i)const;

    // throws fro out of bounds index
    size_t binValue(const size_t)const;

    void addValue( const double& v);

private:

    double m_firstBin;
    double m_binWidth;
    size_t m_binCount;
    size_t m_underflowCount=0;
    size_t m_overflowCount=0;
    size_t m_totalCount=0;

    std::vector<size_t> m_bins;
};

#endif // HISTOGRAM_H
