#include "mathprocessor.h"

#include<limits>
#include<QMap>
#include<cmath>

const double MathProcessor::NULL_VALUE=std::numeric_limits<double>::max();

namespace{

QMap<MathProcessor::OP, QString> op_name_lookup{
    {MathProcessor::OP::SET_V, "Value1"},
    {MathProcessor::OP::SET_G1, "Input1"},
    {MathProcessor::OP::SET_G2, "Input2"},
    {MathProcessor::OP::ADD_GV, "Input1 + Value1"},
    {MathProcessor::OP::SUB_GV, "Input1 - Value1"},
    {MathProcessor::OP::MUL_GV, "Input1 * Value1"},
    {MathProcessor::OP::DIV_GV, "Input1 / Value1"},
    {MathProcessor::OP::POW_GV, "Input1 ** Value1"},
    {MathProcessor::OP::DIV_VG, "Value1 / Input1"},
    {MathProcessor::OP::POW_VG, "Value1 ** Input1"},
    {MathProcessor::OP::ADD_GG, "Input1 + Input2"},
    {MathProcessor::OP::SUB_GG, "Input1 - Input2"},
    {MathProcessor::OP::MUL_GG, "Input1 * Input2"},
    {MathProcessor::OP::DIV_GG, "Input1 / Input2"},
    {MathProcessor::OP::ADD_MUL_GVG, "Input1 + Value1*Input2"},
    {MathProcessor::OP::NORM_GG, "SQRT( Input1**2 + Input2**2 )"},
    {MathProcessor::OP::REL_DIFF_GG, "(Input1 - Input2) / Input2"},
    {MathProcessor::OP::OVERLAY_G2_G1, "Overlay Input2 over Input1"},
    {MathProcessor::OP::CLIP_VV, "Clip Value1 <= Input1 <= Value2"}

};

}

QString MathProcessor::toUserString(QString str, QString inputName){
    return str.replace("Input", inputName);
}

QString MathProcessor::toFunctionString(QString str, QString inputName){
    return str.replace(inputName, "Input");
}

QString MathProcessor::toQString(OP op, QString inputName){
    return toUserString(op_name_lookup.value(op, op_name_lookup.value(OP::SET_V)), inputName);
}

MathProcessor::OP MathProcessor::toOP(QString name, QString inputName){
    return op_name_lookup.key(toFunctionString(name, inputName), OP::SET_V);
}

QStringList MathProcessor::opList(QString inputName){
    QStringList list;
    for( auto str : op_name_lookup.values()){
        list.append(toUserString(str,inputName));
    }
    return list;
}

MathProcessor::MathProcessor() : m_op(OP::SET_V), m_input1(0), m_input2(0),
                                m_value1(0), m_value2(0), m_inputNullValue(NULL_VALUE)
{
    updateFunc();
}

void MathProcessor::setOP( OP op ){

    m_op=op;
    updateFunc();
}

void MathProcessor::setInput1(const double & x){
    m_input1=x;
}

void MathProcessor::setInput2(const double & x){
    m_input2=x;
}

void MathProcessor::setValue1(const double & x){
    m_value1=x;
}

void MathProcessor::setValue2(const double & x){
    m_value2=x;
}

void MathProcessor::setInputNullValue(const double & x){
    m_inputNullValue=x;
}

double MathProcessor::compute(){
    return m_func();
}

double MathProcessor::set_v(){
    return m_value1;
}

double MathProcessor::set_g1(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return m_input1;
}

double MathProcessor::set_g2(){
    if(m_input2==m_inputNullValue) return NULL_VALUE;
    return m_input2;
}


double MathProcessor::add_gv(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return m_input1+m_value1;
}

double MathProcessor::sub_gv(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return m_input1-m_value1;
}

double MathProcessor::mul_gv(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return m_input1*m_value1;
}

double MathProcessor::div_gv(){
    if(m_input1==m_inputNullValue || m_value1==0) return NULL_VALUE;   // ADD EPS for zero checking!!!
    return m_input1/m_value1;
}

double MathProcessor::pow_gv(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return std::pow(m_input1,m_value1);
}

double MathProcessor::div_vg(){
    if(m_input1==m_inputNullValue || !m_input1) return NULL_VALUE;   // ADD EPS for zero checking!!!
    return m_value1/m_input1;
}

double MathProcessor::pow_vg(){
    if(m_input1==m_inputNullValue) return NULL_VALUE;
    return std::pow(m_value1, m_input1);
}

double MathProcessor::add_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return m_input1+m_input2;
}

double MathProcessor::sub_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return m_input1-m_input2;
}

double MathProcessor::mul_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return m_input1*m_input2;
}

double MathProcessor::div_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return (m_input2) ? m_input1/m_input2 : NULL_VALUE;
}

double MathProcessor::add_mul_gvg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return m_input1+m_value1*m_input2;
}

double MathProcessor::norm_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return std::sqrt( m_input1*m_input1 + m_input2*m_input2);
}

double MathProcessor::rel_diff_gg(){
    if(m_input1==m_inputNullValue || m_input2==m_inputNullValue) return NULL_VALUE;
    return (m_input2) ? (m_input1-m_input2)/m_input2 : NULL_VALUE;
}

double MathProcessor::overlay_g2_g1(){
    if(m_input2==m_inputNullValue) return m_input1;
    return m_input2;
}

double MathProcessor::clip_vv(){
    if( m_input1==m_inputNullValue) return m_input1;
    if( m_input1<m_value1) return m_value1;
    if( m_input1>m_value2) return m_value2;
    return m_input1;
}

void MathProcessor::updateFunc(){
    switch(m_op){
    case OP::SET_V: m_func= std::bind(&MathProcessor::set_v, this); break;
    case OP::SET_G1: m_func= std::bind(&MathProcessor::set_g1, this); break;
    case OP::SET_G2: m_func= std::bind(&MathProcessor::set_g2, this); break;
    case OP::ADD_GV: m_func= std::bind(&MathProcessor::add_gv, this); break;
    case OP::SUB_GV: m_func= std::bind(&MathProcessor::sub_gv, this); break;
    case OP::MUL_GV: m_func= std::bind(&MathProcessor::mul_gv, this); break;
    case OP::DIV_GV: m_func= std::bind(&MathProcessor::div_gv, this); break;
    case OP::POW_GV: m_func= std::bind(&MathProcessor::pow_gv, this); break;
    case OP::DIV_VG: m_func= std::bind(&MathProcessor::div_vg, this); break;
    case OP::POW_VG: m_func= std::bind(&MathProcessor::pow_vg, this); break;
    case OP::ADD_GG: m_func= std::bind(&MathProcessor::add_gg, this); break;
    case OP::SUB_GG: m_func= std::bind(&MathProcessor::sub_gg, this); break;
    case OP::MUL_GG: m_func= std::bind(&MathProcessor::mul_gg, this); break;
    case OP::DIV_GG: m_func= std::bind(&MathProcessor::div_gg, this); break;
    case OP::ADD_MUL_GVG: m_func= std::bind(&MathProcessor::add_mul_gvg, this); break;
    case OP::NORM_GG: m_func= std::bind(&MathProcessor::norm_gg, this); break;
    case OP::REL_DIFF_GG: m_func= std::bind(&MathProcessor::rel_diff_gg, this); break;
    case OP::OVERLAY_G2_G1: m_func=std::bind(&MathProcessor::overlay_g2_g1, this); break;
    case OP::CLIP_VV: m_func=std::bind(&MathProcessor::clip_vv, this); break;
    };
}
