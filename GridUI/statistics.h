#ifndef STATISTICS_H
#define STATISTICS_H

#include<iterator>
#include<limits>
#include<cmath>

namespace statistics{

struct Statistics{
    unsigned n;
    double min;
    double max;
    double maxabs;
    double mean;
    double variance;
    double sigma;
    double rms;
};

template<class ITER>
Statistics computeStatistics(ITER begin, ITER end, typename std::iterator_traits<ITER>::value_type NULL_VALUE){
    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    double M=0;     // well's method mean
    double S=0;     // well's method sum
    double sum2=0;  // sum of squares for rms computation
    unsigned n=0;
    for( ITER iter=begin; iter!=end; iter++){
        auto x=*iter;
        if(x==NULL_VALUE || !std::isfinite(x)) continue;
        if(x<min) min=x;
        if(x>max) max=x;
        sum2+=x*x;
        n++;    // increment this first because need it one based for well's method
        auto oldM=M;
        M = M + (x-M)/n;
        S += (x-M)*(x-oldM);
    }
    Statistics stats;
    stats.min=min;
    stats.max=max;
    stats.maxabs=(std::fabs(max)>std::fabs(min))?std::fabs(max):std::fabs(min);
    stats.mean=M;
    stats.rms=(n>0) ? std::sqrt(sum2/n) : 0;
    stats.variance= (n>0) ? S/(n-1) : 0;
    stats.sigma=std::sqrt(stats.variance);
    return stats;
}

}


#endif // STATISTICS_H
