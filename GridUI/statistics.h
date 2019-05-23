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
    double rms;
};

template<class ITER>
Statistics computeStatistics(ITER begin, ITER end, typename std::iterator_traits<ITER>::value_type NULL_VALUE){
    double sum=0;
    double sum2=0;
    double min=std::numeric_limits<double>::max();
    double max=std::numeric_limits<double>::lowest();
    unsigned n=0;
    for( ITER iter=begin; iter!=end; iter++){
        auto x=*iter;
        if(x==NULL_VALUE) continue;
        if(x<min) min=x;
        if(x>max) max=x;
        sum+=x;
        sum2+=x*x;
        n++;
    }
    auto maxabs=(std::fabs(max)>std::fabs(min))?std::fabs(max):std::fabs(min);
    auto mean=sum/n;
    auto rms=std::sqrt(sum2/n);
    return Statistics{n, min, max, maxabs, mean, rms};
}

}


#endif // STATISTICS_H
