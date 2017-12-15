#ifndef GATHERFILTER_H
#define GATHERFILTER_H


#include <gather.h>
#include <trace.h>
#include <memory>
#include <limits>


class GatherFilter
{
public:
    GatherFilter( const double& minOffset, const double& maxOffset,
                  const double& minAzimuth, const double& maxAzimuth);

    double minimumOffset()const{
        return m_minOffset;
    }

    double maximumOffset()const{
        return m_maxOffset;
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

    double m_minOffset;
    double m_maxOffset;
    double m_minAzimuth;
    double m_maxAzimuth;

};

#endif // GATHERFILTER_H
