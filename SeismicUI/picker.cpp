#include "picker.h"

using namespace std::placeholders;

Picker::Picker(QObject *parent) : QObject(parent)
{
    pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
    deletePickFunc = std::bind(&Picker::deleteSingle, this, _1 );
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

void Picker::setMode(PickMode m){

  if( m==m_mode) return;

  m_mode=m;

  updateModeFuncs();

  emit modeChanged(m);
}

void Picker::setType( PickType t){

    if( t==m_type) return;

    m_type=t;

    updateTypeFunc();

    emit typeChanged(t);
}

void Picker::pick( int traceNo, float secs ){

    pickFunc(traceNo, secs);
}

float Picker::pick1(int traceNo, float secs){

    const seismic::Trace& trace=(*m_gather)[traceNo];
    const seismic::Header& header=trace.header();

    int iline=header.at("iline").intValue();
    int xline=header.at("xline").intValue();

    // do nothing if trace is not inside pick grid bounds
    if( !m_picks->bounds().isInside(iline, xline ) ) return m_picks->NULL_VALUE;

    // store NULL as pick if time is not within trace
    if( secs<trace.ft() || secs>trace.lt() ){
        (*m_picks)( iline, xline ) = m_picks->NULL_VALUE;
        return m_picks->NULL_VALUE;
    }
    else{
        secs = adjustPick(trace, secs);
        (*m_picks)(iline, xline) = 1000 * secs;     // picks are stored in milliseconds
        return secs;
    }
}

void Picker::pickSingle( int traceNo, float secs ){

    if( !m_gather || !m_picks ) return;

    pick1( traceNo, secs );

    emit picksChanged();
}



void Picker::pickFillRight(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo<m_gather->size(); traceNo++){

        traceTime = pick1(traceNo, traceTime);

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
}

void Picker::pickFillLeft(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo>=0; traceNo--){

        traceTime = pick1( traceNo, traceTime);

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
}


void Picker::pickFillAll(int firstTraceNo, float traceTime){

    pickFillLeft(firstTraceNo, traceTime);
    pickFillRight(firstTraceNo, traceTime);
}


void Picker::deletePick( int traceNo ){

    deletePickFunc( traceNo );
}

bool Picker::delete1( int traceNo){

    const seismic::Trace& trace=(*m_gather)[traceNo];
    const seismic::Header& header=trace.header();

    int iline=header.at("iline").intValue();
    int xline=header.at("xline").intValue();

    if( !m_picks->bounds().isInside(iline, xline ) ) return false;

    // nothing to delete, allready NULL
    if( (*m_picks)(iline, xline) == m_picks->NULL_VALUE ) return false;

    (*m_picks)(iline, xline) = m_picks->NULL_VALUE;

    return true;
}

void Picker::deleteSingle( int traceNo ){

    if( !m_gather || !m_picks || traceNo<0 || traceNo>=m_gather->size() ) return;

    delete1(traceNo);
    emit picksChanged();
}


void Picker::deleteLeft( int traceNo ){

    if( !m_gather || !m_picks || traceNo<0 ) return;

    for( ; traceNo>=0; traceNo-- ){
        if( !delete1(traceNo) ) break;
    }

    emit picksChanged();
}

void Picker::deleteRight( int traceNo ){

    if( !m_gather || !m_picks || traceNo<0 ) return;

    for( ; traceNo<m_gather->size(); traceNo++ ){
        if( !delete1(traceNo) ) break;
    }

    emit picksChanged();
}

void Picker::deleteAll(int traceNo ){

    deleteLeft( traceNo );
    deleteRight( traceNo + 1 ); // start 1 trace further, otherwise would stop immediately because traceno already NULL
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


float Picker::adjustZero(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = ( t - trace.ft() ) / trace.dt();

    // move up
    int iup=it;
    while( (iup-1>=0) && (sam[iup-1]*sam[iup]>0) ) iup--;

    // move down
    int idown=it;
    while( (idown+1<sam.size()) && (sam[idown+1]*sam[idown]>0) ) idown++;

    // no up movement and start time is not min
    if( iup == it && sam[idown]*sam[it]<0){
        it=idown;
    }
    else if( idown == it && sam[iup]*sam[it]<0){
        it=iup;
    }
    else{ // select closest
        it =  ( it-iup < idown-it ) ? iup : idown;
    }

    return trace.ft() + it * trace.dt();
}

float Picker::adjustNone(const seismic::Trace & trace, float t){

    return t;
}

void Picker::updateModeFuncs(){

    switch( m_mode){
    case PickMode::Single:
        pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteSingle, this, _1 );
        break;
    case PickMode::Left:
        pickFunc = std::bind(&Picker::pickFillLeft, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteLeft, this, _1 );
        break;
    case PickMode::Right:
        pickFunc = std::bind(&Picker::pickFillRight, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteRight, this, _1);
        break;
    case PickMode::All:
        pickFunc = std::bind(&Picker::pickFillAll, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteAll, this, _1);
        break;
    }
}


void Picker::updateTypeFunc(){
    switch( m_type ){
    case PickType::Minimum:
        adjustFunc = std::bind(&Picker::adjustMinimum, this, _1, _2);
        break;
    case PickType::Maximum:
        adjustFunc = std::bind(&Picker::adjustMaximum, this, _1, _2);
        break;
    case PickType::Zero:
        adjustFunc = std::bind(&Picker::adjustZero, this, _1, _2);
        break;
    case PickType::Free:
        adjustFunc = std::bind(&Picker::adjustNone, this, _1, _2);
        break;
    }
}
