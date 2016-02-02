#ifndef GATHERFILTER_H
#define GATHERFILTER_H


#include <gather.h>
#include <trace.h>
#include <memory>



class GatherFilter
{
public:
    GatherFilter( const double& maxABSOffset, const double& minAzimuth=0, const double& maxAzimuth=180);

    double maximumAbsoluteOffset()const{
        return m_maxABSOffset;
    }

    double minimumAzimuth()const{
        return m_minAzimuth;
    }

    double maximumAzimuth()const{
        return m_maxAzimuth;
    }

    std::shared_ptr<seismic::Gather> filter(std::shared_ptr<seismic::Gather>);

private:

    bool isOffsetAccepted(const seismic::Trace& trace);
    bool isAzimuthAccepted(const seismic::Trace& trace);

    double m_maxABSOffset;
    double m_minAzimuth;
    double m_maxAzimuth;

};

#endif // GATHERFILTER_H
