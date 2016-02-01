#include "amplitudecurves.h"



const double RADIANS_TO_DEGREES=57.29577951;


QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              size_t sampleNumber, const double& maximumOffset,
                                            const double& minimumAzimuth, const double& maximumAzimuth){


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

        double sx=trace.header().at("sx").floatValue();
        double sy=trace.header().at("sy").floatValue();
        double gx=trace.header().at("gx").floatValue();
        double gy=trace.header().at("gy").floatValue();
        double azimuth=RADIANS_TO_DEGREES * std::atan2(sx-gx, sy-gy);
        if( azimuth < 0 ){
            azimuth+=180;
        }
        //std::cout<<"sx="<<sx<<" sy="<<sy<<" gx="<<gx<<" gy="<<gy<<" az="<<azimuth<<std::endl;
        if( azimuth<minimumAzimuth || azimuth>maximumAzimuth) continue;

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

QVector<QPointF> buildAmplitudeOffsetCurve( const seismic::Gather& gather,
                                              const double& windowCenterTime,
                                              const double& maximumOffset,
                                              const double& minimumAzimuth,
                                              const double& maximumAzimuth,
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
        // ignore "hard" zeros
        if( amplitude==0 ) continue;

        double offset=trace.header().at("offset").floatValue();
        if( std::fabs(offset)>maximumOffset) continue;

        double sx=trace.header().at("sx").floatValue();
        double sy=trace.header().at("sy").floatValue();
        double gx=trace.header().at("gx").floatValue();
        double gy=trace.header().at("gy").floatValue();
        double azimuth=RADIANS_TO_DEGREES * std::atan2(sx-gx, sy-gy);
        if( azimuth < 0 ){
            azimuth+=180;
        }
 //       std::cout<<"sx="<<sx<<" sy="<<sy<<" gx="<<gx<<" gy="<<gy<<" az="<<azimuth<<std::endl;
        if( azimuth<minimumAzimuth || azimuth>maximumAzimuth) continue;

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


