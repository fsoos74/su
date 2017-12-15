#include "integrationprocessor.h"

#include<limits>
#include<QMap>
#include<cmath>

const double IntegrationProcessor::NULL_VALUE=std::numeric_limits<double>::max();

namespace{

QMap<IntegrationProcessor::OP, QString> op_name_lookup{
    {IntegrationProcessor::OP::SUM, "Running Sum"},
    {IntegrationProcessor::OP::AVG, "Running Average"}
};

}

QString IntegrationProcessor::toQString(OP op){
    return op_name_lookup.value(op, op_name_lookup.value(OP::SUM));
}

IntegrationProcessor::OP IntegrationProcessor::toOP(QString name){
    return op_name_lookup.key(name, OP::SUM);
}

QStringList IntegrationProcessor::opList(){
    return op_name_lookup.values();
}

IntegrationProcessor::IntegrationProcessor() : m_op(OP::SUM), m_input(0), m_sum(0), m_count(0), m_inputNullValue(NULL_VALUE)
{
    updateFunc();
}

void IntegrationProcessor::setOP( OP op ){

    m_op=op;
    updateFunc();
}

void IntegrationProcessor::addValue(const double & x){
    if( x==m_inputNullValue ) return;
    m_sum+=x;
    m_count++;
}


void IntegrationProcessor::restart(const double & x){
    m_sum=x;
    m_count=0;
}

void IntegrationProcessor::setInputNullValue(const double & x){
    m_inputNullValue=x;
}

double IntegrationProcessor::compute(){
    return m_func();
}

double IntegrationProcessor::sum(){
    return m_sum;
}

double IntegrationProcessor::avg(){
    if(!m_count) return NULL_VALUE;
    return m_sum/m_count;
}


void IntegrationProcessor::updateFunc(){
    switch(m_op){
    case OP::SUM: m_func= std::bind(&IntegrationProcessor::sum, this); break;
    case OP::AVG: m_func= std::bind(&IntegrationProcessor::avg, this); break;
    };
}
