#include "windowreductionfunction.h"

#include<trace.h>
#include<seismicdatasetreader.h>
#include <algorithm>
#include<iostream>

ReductionMethod string2ReductionMethod( const QString& str){

   for( auto type : ReductionMethodsAndNames.keys()){
       if( str == ReductionMethodsAndNames.value(type) ){
           return type;
       }
   }

   throw std::runtime_error("Invalid Reduction method name in string2ReductionMethod");
}

QString ReductionMethod2String( ReductionMethod t ){

    for( auto type : ReductionMethodsAndNames.keys()){
        if( t == type ){
            return ReductionMethodsAndNames.value(type);
        }
    }

    throw std::runtime_error("Invalid Reductionmethod  in ReductionMethod2String");
}


struct Sum : public ReductionFunction{
    double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end ){
        if( begin==end) throw std::runtime_error("Trace Reduction called with begin==end");
        double sum=0;
        for( seismic::Trace::Samples::const_iterator it=begin; it!=end; it++){
            sum+=*it;
        }
        return sum;
    }
};

struct Minimum : public ReductionFunction{
    double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end){
        auto it = std::min_element( begin, end);
        if( it==end) throw std::runtime_error("Trace Reduction called with begin==end");
        return *it;
    }
};

struct Maximum : public ReductionFunction{
    double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end ){
        auto it = std::max_element( begin, end);
        if( it==end) throw std::runtime_error("Trace Reduction called with begin==end");
        return *it;
    }
};

struct Mean : public ReductionFunction{
    double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end){
        if( begin==end) throw std::runtime_error("Trace Reduction called with begin==end");
        double sum=0;
        size_t n=0;
        for( seismic::Trace::Samples::const_iterator it=begin; it!=end; it++){
            sum+=*it;
            n++;
        }
        return sum/n;
    }
};

struct RMS : public ReductionFunction{
    double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end){
        if( begin==end) throw std::runtime_error("Trace Reduction called with begin==end");
        double sum=0;
        size_t n=0;
        for( seismic::Trace::Samples::const_iterator it=begin; it!=end; it++){
            sum+=(*it) * (*it);
            n++;
        }
        return std::sqrt( sum/n );
    }
};


std::unique_ptr<ReductionFunction> reductionFunctionFactory( ReductionMethod method){

    switch( method){
    case ReductionMethod::Sum:      return std::unique_ptr<ReductionFunction>(new Sum); break;
    case ReductionMethod::Minimum:  return std::unique_ptr<ReductionFunction>(new Minimum); break;
    case ReductionMethod::Maximum:  return std::unique_ptr<ReductionFunction>(new Maximum); break;
    case ReductionMethod::Mean:     return std::unique_ptr<ReductionFunction>(new Mean); break;
    case ReductionMethod::RMS:      return std::unique_ptr<ReductionFunction>(new RMS); break;
    default: throw std::runtime_error("Illegal Reduction method!"); break;
    }

}
