#ifndef SEGYINFO_H
#define SEGYINFO_H

#include<vector>
#include<string>

#include "segy_header_def.h"
#include "segy_binary_header_def.h"
#include "segy_traceheader_def.h"
#include "segysampleformat.h"

namespace seismic{


class SEGYInfo
{
public:

    SEGYInfo()=default;

    SEGYInfo(const SEGYInfo&)=default;

    bool isSwap()const{
        return m_swap;
    }

    const std::vector<SEGYHeaderWordDef>& binaryHeaderDef()const{
        return m_binaryHeaderDef;
    }

    const std::vector<SEGYHeaderWordDef>& traceHeaderDef()const{
        return m_traceHeaderDef;
    }

    const SEGYHeaderWordDef& scalcoDef()const{
        return m_scalcoDef;
    }

    const SEGYHeaderWordDef& scalelDef()const{
        return m_scalelDef;
    }

    bool isOverrideSampleFormat()const{
        return m_overrideSampleFormat;
    }

    SEGYSampleFormat sampleFormat()const{
        return m_sampleFormat;
    }

    bool isFixedScalco()const{
        return m_fixedScalco;
    }

    double scalco()const{
        return m_scalco;
    }

    bool isFixedScalel()const{
        return m_fixedScalel;
    }

    double scalel()const{
        return m_scalel;
    }


    void setSwap( bool );
    void setBinaryHeaderDef(const std::vector<SEGYHeaderWordDef>&);
    void setTraceHeaderDef(const std::vector<SEGYHeaderWordDef>&);
    void setScalcoDef(const SEGYHeaderWordDef&);
    void setScalelDef(const SEGYHeaderWordDef&);
    void setSampleFormat( SEGYSampleFormat);
    void setScalco( const double&);
    void setScalel( const double&);
    void setOverrideSampleFormat(bool);
    void setFixedScalco( bool);
    void setFixedScalel(bool);

private:

    bool                                m_swap=true;

    std::vector<SEGYHeaderWordDef>      m_binaryHeaderDef=SEGY_DEFAULT_BINARY_HEADER_DEF;
    std::vector<SEGYHeaderWordDef>      m_traceHeaderDef=SEGY_DEFAULT_TRACE_HEADER_DEF;
    SEGYHeaderWordDef                   m_scalelDef=SEGYHeaderWordDef("scalel",
                                                                      SEGYHeaderWordConvType::PLAIN,
                                                                      SEGYHeaderWordDataType::INT16,
                                                                      69,
                                                                      R"( Scalar for elevations and depths (+ = multiplier, - = divisor)
                                                                       )");
    SEGYHeaderWordDef                   m_scalcoDef=SEGYHeaderWordDef("scalco",
                                                                      SEGYHeaderWordConvType::PLAIN,
                                                                      SEGYHeaderWordDataType::INT16,
                                                                      71,
                                                                      R"( Scalar for coordinates (+ = multiplier, - = divisor)
                                                                      )");

    bool                                m_overrideSampleFormat=false;
    SEGYSampleFormat                    m_sampleFormat;

    bool                                m_fixedScalel=false;
    double                              m_scalel=1;

    bool                                m_fixedScalco=false;
    double                              m_scalco=1;
};

}

#endif // SEGYINFO_H
