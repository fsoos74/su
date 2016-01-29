#include "segyinfo.h"

namespace seismic{



void SEGYInfo::setSwap( bool on){
    m_swap=on;
}

void SEGYInfo::setBinaryHeaderDef(const std::vector<SEGYHeaderWordDef>& def){
    m_binaryHeaderDef=def;
}

void SEGYInfo::setTraceHeaderDef(const std::vector<SEGYHeaderWordDef>& def){
    m_traceHeaderDef=def;
}

void SEGYInfo::setScalcoDef(const SEGYHeaderWordDef& def){
    m_scalcoDef=def;
}

void SEGYInfo::setScalelDef(const SEGYHeaderWordDef& def){
    m_scalelDef=def;
}

void SEGYInfo::setSampleFormat( SEGYSampleFormat f){
    m_overrideSampleFormat=true;
    m_sampleFormat=f;
}

void SEGYInfo::setScalco( const double& s){
    m_fixedScalco=true;
    m_scalco=s;
}

void SEGYInfo::setScalel( const double& s){
    m_fixedScalel=true;
    m_scalel=s;
}

void SEGYInfo::setOverrideSampleFormat(bool on){
    m_overrideSampleFormat=on;
}

void SEGYInfo::setFixedScalco( bool on){
    m_fixedScalco=on;
}

void SEGYInfo::setFixedScalel(bool on){
    m_fixedScalel=on;
}


}
