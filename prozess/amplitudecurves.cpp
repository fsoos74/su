#include "amplitudecurves.h"


QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              size_t sampleNumber, const double& maximumOffset ){


    QVector<QPointF> curve;

    if( gather.empty() ){
        return curve;
    }

    curve.reserve( gather.size());

    for( const seismic::Trace& trace : gather){

        if( std::fabs(trace.header().at("offset").floatValue())>maximumOffset) continue;

        if( sampleNumber+1>trace.samples().size()) continue;        // silently skip out of range sample indices

        double amplitude=trace.samples()[sampleNumber];
        double offset=trace.header().at("offset").floatValue();

        // ignore "hard" zeros
        if( amplitude==0 ) continue;

        curve.push_back(QPointF(offset, amplitude ) );
    }

    return curve;
}



QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              const double& maximumOffset,
                                              ReductionFunction* rf, int windowSize ){


    QVector<QPointF> curve;

    if( gather.empty() ){
        return curve;
    }

    const double& t=windowCenterTime;    // horizon in millis

    curve.reserve( gather.size());

    for( const seismic::Trace& trace : gather){

        if( std::fabs(trace.header().at("offset").floatValue())>maximumOffset) continue;

        size_t horizonIndex=trace.time_to_index(t); // returns trace.samples.size() if time is out of trace bounds

        if( ( horizonIndex < windowSize/2 ) || ( horizonIndex + windowSize/2 >=trace.samples().size() ) ) continue;

        seismic::Trace::Samples::const_iterator begin=trace.samples().cbegin() + horizonIndex - windowSize/2;
        seismic::Trace::Samples::const_iterator end=trace.samples().cbegin() + horizonIndex + windowSize/2 + 1;

        double amplitude= (*rf)(begin, end);
        double offset=trace.header().at("offset").floatValue();

        // ignore "hard" zeros
        if( amplitude==0 ) continue;

        curve.push_back(QPointF(offset, amplitude ) );
    }

    return curve;
}



void convertOffsetCurveToAngle( QVector<QPointF>& curve, const double& depth){

    for( QPointF& p : curve){

        double theta=std::atan2(p.x(), 2*depth);     // atan2 should take care of division be zero
        p.setX(theta);
    }

}


QVector<QPointF> buildAmplitudeAngleCurve( const seismic::Gather& gather,
                                              size_t sampleNumber, const double& maximumOffset,
                                              const double& depth){


    QVector<QPointF> curve=buildAmplitudeOffsetCurve(gather, sampleNumber, maximumOffset );

    convertOffsetCurveToAngle(curve, depth);

    return curve;
}



QVector<QPointF> buildAmplitudeAngleCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              const double& maximumOffset,
                                              ReductionFunction* rf, int windowSize,
                                           const double& depth){


    QVector<QPointF> curve=buildAmplitudeOffsetCurve(gather, windowCenterTime, maximumOffset, rf, windowSize );

    convertOffsetCurveToAngle(curve, depth);

    return curve;
}


