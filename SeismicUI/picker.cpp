#include "picker.h"

//#include <interp.h>


using namespace std::placeholders;


double lininterp(double x1, double y1, double x2, double y2, double x){

    // need to add eps checks instead ==


   // if( x<=x1 ) return y1;
   // if( x>=x2 ) return y2;
    if( x1 == x2 ) return (y1+y2)/2;

    return y1 + x * ( y2 - y1 ) / ( x2 - x1 );
}

Picker::Picker(QObject *parent) : QObject(parent)
{
    pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
    deletePickFunc = std::bind(&Picker::deleteSingle, this, _1 );
    adjustFunc = std::bind(&Picker::adjustMinimum, this, _1, _2);
}


void Picker::setGather( std::shared_ptr<seismic::Gather>  g ){

    if( g==m_gather ) return;

    m_gather=g;

    if( m_fillMode!=PickFillMode::Next){
        fillILXLBuffer();
    }

    emit gatherChanged();
}

void Picker::setPicks( std::shared_ptr<Grid2D<float>> p ){

    if( m_picks == p ) return;

    m_picks=p;

    m_dirty=false;

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

void Picker::setFillMode(PickFillMode m){

    if( m==m_fillMode) return;

    m_fillMode=m;

    updateModeFuncs();

    if( m!=PickFillMode::Next ){        // need to fill buffer on current gather since this was not done yet
        fillILXLBuffer();
    }

    emit fillModeChanged(m);
}

void Picker::setConservative(bool on){

    if( on==m_conservative) return;

    m_conservative=on;

    emit conservativeChanged(on);
}

void Picker::setDirty(bool on){
    m_dirty=on;
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
         m_dirty=true;
        (*m_picks)( iline, xline ) = m_picks->NULL_VALUE;       
        return m_picks->NULL_VALUE;
    }
    else{

        // don't overwrite existing picks in conservative mode
        if( m_conservative && (*m_picks)(iline,xline)!=m_picks->NULL_VALUE ) return m_picks->NULL_VALUE;

        m_dirty=true;
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

template<typename T>
inline T SQR( T x ){ return x*x; }


void Picker::pickFillRightNearest(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo<m_gather->size(); traceNo++){

        if( traceNo == firstTraceNo ){ //|| m_ilxlBuffer[traceNo].first==m_ilxlBuffer[traceNo-1].first){

            traceTime = pick1(traceNo, traceTime);
        }
       else{  // search for nearest trace that has been picked

            int iline = m_ilxlBuffer[traceNo].first;
            int xline = m_ilxlBuffer[traceNo].second;
            int near_trace = traceNo - 1;   // first guess previous trace
            int near_dist2 = SQR(m_ilxlBuffer[near_trace].first - iline) + SQR(m_ilxlBuffer[near_trace].second - xline);

            for( int i = near_trace - 1; i > firstTraceNo; i--){
                int dist2 = SQR(m_ilxlBuffer[i].first - iline ) + SQR(m_ilxlBuffer[i].second - xline);
                if( dist2 < near_dist2 ){
                    near_trace = i;
                    near_dist2 = dist2;
                }
            }


            float prev_time=(*m_picks)( m_ilxlBuffer[near_trace].first, m_ilxlBuffer[near_trace].second);
            prev_time*=0.001;   // ms -> sec

            traceTime = pick1(traceNo, prev_time);
        }

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
}

// original
void Picker::pickFillRightNext(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo<m_gather->size(); traceNo++){

        traceTime = pick1(traceNo, traceTime);

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
}


void Picker::pickFillLeftNearest(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo>=0; traceNo--){

        if( traceNo == firstTraceNo ){      // pick first trace

            traceTime = pick1(traceNo, traceTime);
        }
       else{  // search for nearest trace that has been picked

            int iline = m_ilxlBuffer[traceNo].first;
            int xline = m_ilxlBuffer[traceNo].second;
            int near_trace = traceNo + 1;   // first guess previous trace
            int near_dist2 = SQR(m_ilxlBuffer[near_trace].first - iline) + SQR(m_ilxlBuffer[near_trace].second - xline);

            for( int i = near_trace + 1; i <=firstTraceNo; i++){
                int dist2 = SQR(m_ilxlBuffer[i].first - iline ) + SQR(m_ilxlBuffer[i].second - xline);
                if( dist2 < near_dist2 ){
                    near_trace = i;
                    near_dist2 = dist2;
                }
            }

            float prev_time=(*m_picks)( m_ilxlBuffer[near_trace].first, m_ilxlBuffer[near_trace].second);
            prev_time*=0.001;   // ms -> sec

            traceTime = pick1(traceNo, prev_time);
        }

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
}



void Picker::pickFillLeftNext(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    for( int traceNo = firstTraceNo; traceNo>=0; traceNo--){

        traceTime = pick1( traceNo, traceTime);

        if( traceTime == m_picks->NULL_VALUE) break;
    }

    emit picksChanged();
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

    // nothing to delete, allready NULL, stop here in conservative mode
    if( m_conservative && (*m_picks)(iline, xline) == m_picks->NULL_VALUE ) return false;

    m_dirty=true;

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


float Picker::adjustPick(const seismic::Trace & trace, float t){

    return adjustFunc(trace, t);
}

// testing to find "real" minimum in both directions
float Picker::adjustMinimum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = ( t - trace.ft() ) / trace.dt();

    // check if this is already minimum
    if( it>0 && it+1<sam.size() && sam[it-1]>sam[it] && sam[it+1]>sam[it]) return t;

    // move up
    int iup=it;
    float dir=0;
    if( iup > 0 ) dir = sam[iup-1] - sam[iup];

    while( iup>0 ){

        if( ( dir < 0 ) && ( sam[ iup - 1 ] > sam[iup] )  ) break;
        if( ( dir > 0 ) && ( sam[ iup - 1 ] < sam[iup] )  ){
            dir = sam[ iup - 1 ] - sam[iup];
        }
        iup--;
    }

    // move down
    int idown=it;
    dir=0;
    if( idown + 1 < sam.size() ) dir = sam[idown+1] - sam[idown];

    while( idown+1<sam.size() ){

        if( ( dir < 0 ) && ( sam[ idown + 1 ] > sam[idown] )  ) break;
        if( ( dir > 0 ) && ( sam[ idown + 1 ] < sam[idown] )  ){
            dir = sam[ idown + 1 ] - sam[idown];
        }
        idown++;
    }

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

/* ORIGINAL
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

*/

/* ORIGINAL
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
*/

// testing to find "real" maximum in both directions
float Picker::adjustMaximum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = ( t - trace.ft() ) / trace.dt();

    // check if this is already maximum
    if( it>0 && it+1<sam.size() && sam[it-1]<sam[it] && sam[it+1]<sam[it]) return t;

    // move up
    int iup=it;
    float dir=0;
    if( iup > 0 ) dir = sam[iup-1] - sam[iup];

    while( iup>0 ){

        if( ( dir > 0 ) && ( sam[ iup - 1 ] < sam[iup] )  ) break;
        if( ( dir < 0 ) && ( sam[ iup - 1 ] > sam[iup] )  ){
            dir = sam[ iup - 1 ] - sam[iup];
        }
        iup--;
    }

    // move down
    int idown=it;
    dir=0;
    if( idown + 1 < sam.size() ) dir = sam[idown+1] - sam[idown];

    while( idown+1<sam.size() ){

        if( ( dir > 0 ) && ( sam[ idown + 1 ] < sam[idown] )  ) break;
        if( ( dir < 0 ) && ( sam[ idown + 1 ] > sam[idown] )  ){
            dir = sam[ idown + 1 ] - sam[idown];
        }
        idown++;
    }

    // no up movement and start time is not max
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

    /*
    // no up movement and start time is not min
    if( iup == it && sam[idown]*sam[it]<0){
        it=idown;
    }
    else if( idown == it && sam[iup]*sam[it]<0){
        it=iup;
    }
    else{
 */// select closest
        if( it-iup < idown-it ){
            it = iup;
        }
        else{
            it=idown;
        }

        return trace.ft() + it * trace.dt();
    //}

   // return trace.ft() + it * trace.dt();
}

float Picker::adjustNone(const seismic::Trace & trace, float t){

    return t;
}


void Picker::fillILXLBuffer(){

    m_ilxlBuffer.clear();   // check if this resizes, we don't want resize

    if( !m_gather ){
        return;
    }

    m_ilxlBuffer.reserve(m_gather->size());
    for( int traceNo = 0; traceNo<m_gather->size(); traceNo++){
        const seismic::Trace& trace=(*m_gather)[traceNo];
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();
        m_ilxlBuffer.push_back(std::make_pair(iline,xline));
    }

}

void Picker::updateModeFuncs(){

    switch( m_mode){

    case PickMode::Single:
        pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteSingle, this, _1 );
        break;

    case PickMode::Left:
        switch(m_fillMode){
        case PickFillMode::Next: pickFunc = std::bind(&Picker::pickFillLeftNext, this, _1, _2); break;
        case PickFillMode::Nearest: pickFunc = std::bind(&Picker::pickFillLeftNearest, this, _1, _2); break;
        default: qFatal("Unhandled PickFillMode"); break;
        }

        deletePickFunc = std::bind(&Picker::deleteLeft, this, _1 );
        break;

    case PickMode::Right:
        switch(m_fillMode){
        case PickFillMode::Next: pickFunc = std::bind(&Picker::pickFillRightNext, this, _1, _2); break;
        case PickFillMode::Nearest: pickFunc = std::bind(&Picker::pickFillRightNearest, this, _1, _2); break;
        default: qFatal("Unhandled PickFillMode"); break;
        }

        deletePickFunc = std::bind(&Picker::deleteRight, this, _1);
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
