#include "gatherfilter.h"
#include <trace.h>
#include<cmath>



GatherFilter::GatherFilter( const double& maxABSOffset, const double& minAzimuth, const double& maxAzimuth):
    m_maxABSOffset(maxABSOffset), m_minAzimuth(minAzimuth), m_maxAzimuth(maxAzimuth){

}


std::shared_ptr<seismic::Gather> GatherFilter::filter(std::shared_ptr<seismic::Gather> in){

    std::shared_ptr<seismic::Gather> out(new seismic::Gather);

    if( !in || in->empty() ) return out;

    for( const seismic::Trace& trace : *in){

        if( isOffsetAccepted(trace) && isAzimuthAccepted(trace)){
            out->push_back(trace);
        }

    }

    return out;
}

bool GatherFilter::isOffsetAccepted(const seismic::Trace& trace){

    double offset=trace.header().at("offset").floatValue();

    return (std::fabs(offset)<=m_maxABSOffset);
}

bool GatherFilter::isAzimuthAccepted(const seismic::Trace& trace){

    const double RADIANS_TO_DEGREES=57.29577951;

    double sx=trace.header().at("sx").floatValue();
    double sy=trace.header().at("sy").floatValue();
    double gx=trace.header().at("gx").floatValue();
    double gy=trace.header().at("gy").floatValue();
    double azimuth=RADIANS_TO_DEGREES * std::atan2(sx-gx, sy-gy);
    if( azimuth < 0 ){
        azimuth+=180;
    }
    //std::cout<<"sx="<<sx<<" sy="<<sy<<" gx="<<gx<<" gy="<<gy<<" az="<<azimuth<<std::endl;
    return ( azimuth>=m_minAzimuth && azimuth<=m_maxAzimuth);

}
