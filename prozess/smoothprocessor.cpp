#include "smoothprocessor.h"

#include<limits>
#include<QMap>
#include<cmath>

const double SmoothProcessor::NULL_VALUE=std::numeric_limits<double>::max();

namespace{

QMap<SmoothProcessor::OP, QString> op_name_lookup{
    {SmoothProcessor::OP::MEAN, "Mean"},
    {SmoothProcessor::OP::MEAN80, "Trimmed Mean 80 Percent"},
    {SmoothProcessor::OP::MEAN_DISTANCE_WEIGHTED, "Distance Weighted Mean"},
    {SmoothProcessor::OP::MEDIAN, "Median"} };
}

QString SmoothProcessor::toQString(OP op){
    return op_name_lookup.value(op, op_name_lookup.value(OP::MEAN));
}

SmoothProcessor::OP SmoothProcessor::toOP(QString name){
    return op_name_lookup.key(name, OP::MEAN);
}

QStringList SmoothProcessor::opList(){
    return op_name_lookup.values();
}

SmoothProcessor::SmoothProcessor() : m_op(OP::MEAN), m_inputNullValue(NULL_VALUE)
{
    updateFunc();
}

void SmoothProcessor::setOP( OP op ){

    m_op=op;
    updateFunc();
}

void SmoothProcessor::addInput(const double &value, const double &dist){
    m_data.push_back(std::make_pair(value, dist));
}

void SmoothProcessor::clearInput(){
    m_data.clear();
}

void SmoothProcessor::setInputNullValue(const double & x){
    m_inputNullValue=x;
}

double SmoothProcessor::compute(){
    return m_func();
}

double SmoothProcessor::mean(){
    double sum=0;
    size_t n=0;
    for( auto d : m_data ){
        if( d.first!=m_inputNullValue){
            sum+=d.first;
            n++;
        }
    }

    return (n>0) ? sum/n : NULL_VALUE;
}


double SmoothProcessor::mean_80(){

    std::vector<double> buf;
    buf.reserve(m_data.size());
    for( auto d : m_data){
        if( d.first !=m_inputNullValue ) buf.push_back(d.first);
    }

    if( buf.empty()){
        return NULL_VALUE;
    }

    std::sort(buf.begin(), buf.end());

    double keep=0.8;
    int start=0.5*(1.0-keep)*buf.size();
    int stop=buf.size()-start;  // symmetric
    double sum=std::accumulate( &buf[start], &buf[stop], 0);
    auto n=stop-start+1;

    return sum/n;
}


double SmoothProcessor::mean_distance_weighted(){

    double sum=0;
    double wsum=0;
    size_t n=0;

    for( auto d : m_data){
        if( d.first == m_inputNullValue ) continue;
        double w= 1. / d.second;
        wsum += w;
        sum += w*d.first;
        n++;
    }

    return (wsum>0) ? sum/wsum : NULL_VALUE;
}

double SmoothProcessor::median(){

    std::vector<double> buf;
    buf.reserve(m_data.size());
    for( auto d : m_data){
        if( d.first !=m_inputNullValue ) buf.push_back(d.first);
    }

    if( buf.empty()){
        return NULL_VALUE;
    }

    auto it=buf.begin() + buf.size()/2;
    std::nth_element( buf.begin(), it, buf.end() );

    return *it;
}

void SmoothProcessor::updateFunc(){
    switch(m_op){
        case OP::MEAN: m_func= std::bind(&SmoothProcessor::mean, this); break;
        case OP::MEAN80: m_func= std::bind(&SmoothProcessor::mean_80, this); break;
        case OP::MEAN_DISTANCE_WEIGHTED: m_func= std::bind(&SmoothProcessor::mean_distance_weighted, this); break;
        case OP::MEDIAN: m_func= std::bind(&SmoothProcessor::median, this); break;
    };
}
