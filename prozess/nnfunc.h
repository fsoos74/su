#ifndef NNFUNC_H
#define NNFUNC_H

#include "matrix.h"


inline double sigmoid(double x){
	return 1./(1.+std::exp(-x));
}

inline double dx_sigmoid(double x){
	return x*(1.-x);
}

inline double relu(double x){
    return std::max(0.,x);
}

inline double relu_prime( double x ){
    return (x>0) ? 1:0;
}

inline double leaky_relu(double x){
    return ( x>0) ? x : 0.01*x;
}

inline double leaky_relu_prime( double x ){
    return (x>0) ? 1:0.01;
}



template<class Iter>
void norm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE=std::numeric_limits<double>::max()){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if(x!=NULL_VALUE) x=(x-r.first)/(r.second-r.first);});
}

template<class Iter>
void unnorm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE=std::numeric_limits<double>::max()){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if(x!=NULL_VALUE) x=r.first + x*(r.second-r.first);});
}

#endif
