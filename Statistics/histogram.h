#ifndef HISTOGRAM_H
#define HISTOGRAM_H


#include<vector>
#include<cstddef>   //size_t
#include<algorithm>
#include<limits>

class Histogram
{

public:

    typedef std::vector<size_t>::iterator iterator;
    typedef std::vector<size_t>::const_iterator const_iterator;


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

    std::pair<double, double> range()const{
        return std::make_pair( m_firstBin, m_firstBin + m_binWidth * m_binCount );
    }

    // bounds of bin [min,max), throws error for out of bounds i
    std::pair<double,double> binInterval(const size_t i)const;

    // throws for out of bounds index
    size_t binValue(const size_t)const;

    void addValue( const double& v);

    template<typename ITERATOR> void addValues( ITERATOR begin, ITERATOR end){

        for( ITERATOR it = begin; it!=end; ++it ){
            addValue(*it);
        }
    }


    template<typename ITERATOR, typename VALUE> void addValues( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE){

        for( ITERATOR it = begin; it!=end; ++it ){
            if( *it!=NULL_VALUE) addValue(*it);
        }
    }


    iterator begin(){
        return m_bins.begin();
    }

    iterator end(){
        return m_bins.end();
    }

    const_iterator cbegin()const{
        return m_bins.cbegin();
    }

    const_iterator cend()const{
        return m_bins.cend();
    }

private:

    double m_firstBin;
    double m_binWidth;
    size_t m_binCount;
    size_t m_underflowCount=0;
    size_t m_overflowCount=0;
    size_t m_totalCount=0;

    std::vector<size_t> m_bins;
};

template<typename ITERATOR, typename VALUE>
Histogram createHistogram( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, size_t STEPS ){

    auto min=std::numeric_limits<VALUE>::max();
    auto max=std::numeric_limits<VALUE>::min();
    for( auto it = begin; it!=end; ++it ){
        if( *it==NULL_VALUE) continue;
        if( *it<min ) min=*it;
        if( *it>max ) max=*it;
    }

    if( min>=max ) return Histogram();

    Histogram histogram( min, (max-min)/STEPS, STEPS);

    histogram.addValues( begin, end, NULL_VALUE );

    return histogram;
}


#endif // HISTOGRAM_H
