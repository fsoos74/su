#include "gatherscaler.h"

#include<algorithm>

GatherScaler::GatherScaler(QObject* parent):QObject(parent)
{

}

void GatherScaler::setMode(GatherScaler::Mode mode){

    if(mode==m_mode) return;

    m_mode=mode;

    emit changed();
}

void GatherScaler::setFixedScaleFactor(qreal fac){

    if( fac == m_fixedScaleFactor) return;

    m_fixedScaleFactor=fac;

    emit changed();
}

QVector<qreal> GatherScaler::computeScalingFactors( std::shared_ptr<seismic::Gather> gather){

    if( !gather ) return QVector<qreal>();

    if( m_mode==Mode::NoScaling){
        return QVector<qreal>(gather->size(), 1);   // for now
    }

    if( m_mode==Mode::Fixed){
        return QVector<qreal>(gather->size(), m_fixedScaleFactor);
    }

    QVector<qreal> traceMinimum;
    QVector<qreal> traceMaximum;

    traceMinimum.reserve(gather->size());
    traceMaximum.reserve(gather->size());

    for( size_t i=0; i<gather->size(); i++){

        const seismic::Trace::Samples& samples=(*gather)[i].samples();

        qreal currentMinimum=std::numeric_limits<qreal>::max();
        qreal currentMaximum=std::numeric_limits<qreal>::lowest();
        for( auto sample : samples){
            if( sample<currentMinimum){
                    currentMinimum=sample;
            }
            if( sample>currentMaximum){
                    currentMaximum=sample;
            }
        }

        traceMinimum.push_back( currentMinimum);
        traceMaximum.push_back( currentMaximum);
    }

    if( m_mode==Mode::Individual ){

        QVector<qreal> factors(gather->size(), m_fixedScaleFactor);

        for( int i=0; i<factors.size(); i++){
            qreal absmax=std::max( std::fabs(traceMinimum[i]), std::fabs(traceMaximum[i]));
            factors[i]=(absmax)>0 ? factors[i]/absmax : 0;
        }
        return factors;
    }

    // once we reached here it must be entire scaling

    qreal gatherMinimum=std::numeric_limits<qreal>::max();
    for( auto m : traceMinimum){
        if( m<gatherMinimum){
            gatherMinimum=m;
        }
    }

    qreal gatherMaximum=std::numeric_limits<qreal>::lowest();
    for( auto m : traceMaximum){
        if( m>gatherMaximum){
            gatherMaximum=m;
        }
    }

    qreal absmax=std::max( std::fabs(gatherMinimum), std::fabs(gatherMaximum));
    if( absmax>0){
        return QVector<qreal>(gather->size(), m_fixedScaleFactor/absmax);
    }
    else{
        return QVector<qreal>(gather->size(),0);
    }
}
