#include "picker.h"

using namespace std::placeholders;

Picker::Picker(QObject *parent) : QObject(parent)
{
    pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
    adjustFunc = std::bind(&Picker::adjustMinimum, this, _1, _2);
}


void Picker::setGather( std::shared_ptr<seismic::Gather>  g ){

    if( g==m_gather ) return;

    m_gather=g;

    emit gatherChanged();
}

void Picker::setPicks( std::shared_ptr<Grid2D<float>> p ){

    if( m_picks == p ) return;

    m_picks=p;

    emit picksChanged();
}

void Picker::setMode(Mode m){

  if( m==m_mode) return;

  m_mode=m;

  updateModeFunc();

  emit modeChanged(m);
}

void Picker::setType( Type t){

    if( t==m_type) return;

    m_type=t;

    updateTypeFunc();

    emit typeChanged(t);
}

void Picker::pick( int traceNo, float secs ){

    pickFunc(traceNo, secs);
}

void Picker::pickSingle( int traceNo, float secs ){

    if( !m_gather || !m_picks ) return;

    const seismic::Trace& trace=m_gather->at(traceNo);
    const seismic::Header& header=trace.header();

    int iline=header.at("iline").intValue();
    int xline=header.at("xline").intValue();

    // do nothing if trace is not inside pick grid bounds
    if( !m_picks->bounds().isInside(iline, xline ) ) return;

    // store NULL as pick if time is not within trace
    if( secs<trace.ft() || secs>trace.lt() ){
        (*m_picks)( iline, xline ) = m_picks->NULL_VALUE;
    }
    else{
        secs = adjustPick(trace, secs);
        (*m_picks)(iline, xline) = 1000 * secs;     // picks are stored in milliseconds
    }

    emit picksChanged();
}


void Picker::pickMulti(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks ) return;

    for( int traceNo = firstTraceNo; traceNo<m_gather->size(); traceNo++){

        const seismic::Trace& trace=m_gather->at(traceNo);
        const seismic::Header& header=trace.header();

        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        if( !m_picks->bounds().isInside(iline, xline ) ) break;

        traceTime = adjustPick(trace, traceTime);
        (*m_picks)(iline, xline) = 1000 * traceTime;     // picks are stored in milliseconds
    }
}


float Picker::adjustPick(const seismic::Trace & trace, float t){

    return adjustFunc(trace, t);
}

float Picker::adjustMinimum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = ( t - trace.ft() ) / trace.dt();

    // move up
    int iup=it;
    while( (iup-1>=0) && (sam[iup-1]<sam[iup]) ) iup--;

    // move down
    int idown=it;
    while( (idown+1<sam.size()) && (sam[idown+1]<sam[idown]) ) idown++;

    // no up movement and start time is not min
    if( iup == it && sam[idown]<sam[it]){
        it=idown;
    }
    else if( idown == it && sam[iup]<sam[it]){
        it=iup;
    }
    else{ // select closest
        it =  ( it-iup < idown-it ) ? iup : idown;
    }

    return trace.ft() + it * trace.dt();
}


float Picker::adjustMaximum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = ( t - trace.ft() ) / trace.dt();

    // move up
    int iup=it;
    while( (iup-1>=0) && (sam[iup-1]>sam[iup]) ) iup--;

    // move down
    int idown=it;
    while( (idown+1<sam.size()) && (sam[idown+1]>sam[idown]) ) idown++;

    // no up movement and start time is not min
    if( iup == it && sam[idown]>sam[it]){
        it=idown;
    }
    else if( idown == it && sam[iup]>sam[it]){
        it=iup;
    }
    else{ // select closest
        it =  ( it-iup < idown-it ) ? iup : idown;
    }

    return trace.ft() + it * trace.dt();
}

void Picker::updateModeFunc(){

    switch( m_mode){
    case MODE_SINGLE:
        pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
        break;
    case MODE_MULTI:
        pickFunc = std::bind(&Picker::pickMulti, this, _1, _2);
        break;
    default:    // for now no op, make enum class to avoid this
        break;
    }
}


void Picker::updateTypeFunc(){
    switch( m_type ){
    case TYPE_MINIMUM:
        adjustFunc = std::bind(&Picker::adjustMinimum, this, _1, _2);
        break;
    case TYPE_MAXIMUM:
        adjustFunc = std::bind(&Picker::adjustMaximum, this, _1, _2);
        break;
    default:    // for now no op, make enum class to avoid this
        break;
    }
}
