#include "picker.h"

#include<limits>
#include<tuple>
#include<QList>
#include<QProgressDialog>
#include<QApplication>     // processevents


using namespace std::placeholders;

const float NO_PICK=std::numeric_limits<float>::lowest();


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
    deletePickFunc = std::bind(&Picker::deleteSingle, this, _1, _2 );
    adjustFunc = std::bind(&Picker::adjustMinimum, this, _1, _2);
}


void Picker::newPicks( const QString& key1, const QString& key2, bool multi){

    m_picks=std::make_shared<Table>(key1, key2, multi);
    setDirty(false);
    emit picksChanged();
}

void Picker::setGather( std::shared_ptr<seismic::Gather>  g ){

    if( g==m_gather ) return;

    clearBuffer();

    m_gather=g;
    fillILXLBuffer();

    emit gatherChanged();
}

void Picker::setPicks( std::shared_ptr<Table> p ){

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

void Picker::clearBuffer(){
    m_bufferedPoints.clear();
}

void Picker::finishedBuffer(){

    if( m_mode==PickMode::Outline){
        fillOutline();
    }
    else if(m_mode==PickMode::Lines){
        fillLines();
    }

    m_bufferedPoints.clear();
}

void Picker::pick( int traceNo, float secs ){
    if( traceNo<0 || traceNo>=static_cast<int>(m_gather->size() ) ) return;
    pickFunc(traceNo, secs);
}

float Picker::pick1(int traceNo, float secs, bool adjust){

    if( traceNo<0 || traceNo>=static_cast<int>(m_gather->size() ) )
        throw std::runtime_error("pick1 called with invalid traceno");

    const seismic::Trace& trace=(*m_gather)[traceNo];
    int iline=traceIline(traceNo);
    int xline=traceXline(traceNo);

    // don't overwrite existing picks in conservative mode
    if( m_conservative && m_picks->contains(iline,xline) ) return NO_PICK;

    m_dirty=true;

    if( adjust ) secs = adjustPick(trace, secs);
    m_picks->insert(iline, xline, secs);     // picks are stored in seconds
    return secs;
}

void Picker::pickLines(int traceNo, float secs){
    m_bufferedPoints.push_back(std::make_pair(traceNo, secs));
    //pick1(traceNo, secs, false);        // give feedback, this pick would be later as part of line anyway
    emit bufferedPointsChanged();
}

void Picker::pickOutline(int traceNo, float secs){
    m_bufferedPoints.push_back(std::make_pair(traceNo, secs));
    //pick1(traceNo, secs, false);        // give feedback, this pick would be later as part of outline anyway
    emit bufferedPointsChanged();
}

void Picker::fillOutline(){

    QVector<QPointF> points;
    for( auto p : m_bufferedPoints){
        points<<QPointF(p.first, p.second);
    }
    points<<QPointF(m_bufferedPoints.front().first, m_bufferedPoints.front().second);       // connect first and last point

    QPolygonF poly(points);

    auto bb=poly.boundingRect();
    auto trc0=static_cast<int>( std::floor(bb.left()));
    if( trc0<0 ) trc0=0;
    auto trc1=static_cast<int>( std::ceil(bb.right()));
    if( trc1>m_gather->size()) trc1=m_gather->size();
    auto t0=bb.top();
    auto t1=bb.bottom();

    // fill outline with picks
    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later

    QProgressDialog progress("Adding picks...", "Cancel", t0, t1, dynamic_cast<QWidget*>(parent()) );
    progress.setMinimum(t0);
    progress.setMaximum(t1);
    progress.setValue(t0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setWindowTitle("Filling Outline");
    progress.setUpdatesEnabled(true);
    progress.show();
    qApp->processEvents();


    for( int traceNo=trc0; traceNo<trc1; traceNo++){

        const seismic::Trace& trace=(*m_gather)[traceNo];
        int iline=traceIline(traceNo);
        int xline=traceXline(traceNo);
        auto i0=static_cast<int>(std::floor((t0-trace.ft())/trace.dt()));
        if( i0<0 ) i0=0;
        auto i1=static_cast<int>(std::ceil((t1-trace.ft())/trace.dt()));
        if( i1>trace.size() ) i1=trace.size();
        for( int i=i0; i<i1; i++){
            auto secs=trace.ft()+i*trace.dt();
            if( poly.containsPoint(QPointF(traceNo, secs), Qt::OddEvenFill)){
                buffer.push_back(std::make_tuple(iline, xline, secs));
            }
        }
        progress.setValue(traceNo);

        qApp->processEvents();
        if( progress.wasCanceled()) return;
    }

    m_picks->insert(buffer);

    progress.setValue(t1);
}

void Picker::fillLines(){

    for( int i=1; i<m_bufferedPoints.size(); i++){
        auto pl=m_bufferedPoints[i-1];
        auto xl=static_cast<double>(pl.first);
        auto yl=pl.second;
        auto pr=m_bufferedPoints[i];
        auto xr=static_cast<double>(pr.first);
        auto yr=pr.second;
        auto j0=static_cast<int>(std::ceil(xl));
        auto j1=static_cast<int>(std::floor(xr));
        for( auto j=j0; j<j1; j++){
           auto x=static_cast<double>(j);
           auto y=yl + (x-xl)*(yr-yl)/(xr-xl);
           // const seismic::Trace& trace=(*m_gather)[j];
           // int iline=m_ilxlBuffer[j].first;
           // int xline=m_ilxlBuffer[j].second;
           // m_picks->insert(iline, xline, y);
           pick1( j, y );       // use adjust func
        }
    }

    emit picksChanged();
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

    if( !useTrace(firstTraceNo)) return;        // first trace is zero trace, no pick here

    // pick first trace
    pick1(firstTraceNo, traceTime);

    for( int traceNo = firstTraceNo+1; traceNo<m_gather->size(); traceNo++){

        if( !useTrace(traceNo)) continue;       // skip over zero traces

        int iline = traceIline(traceNo);
        int xline = traceXline(traceNo);

        // search for nearest non-zero trace, only use previous traces in gather since they have been already picked
        int near_trace = -1;
        int near_dist2 = std::numeric_limits<int>::max();
        for( int i = traceNo - 1; i >=0; i--){

            if( !useTrace(i)) continue;     // skip over zero trace

            int iline2 = traceIline(i);
            int xline2 = traceXline(i);
            if( !m_picks->contains(iline2, xline2)) continue;       // no pick for this trace, skip it
            int dist2 = SQR(iline2 - iline ) + SQR(xline2 - xline);
            if( dist2 < near_dist2 ){
                near_trace = i;
                near_dist2 = dist2;
            }
        }

        if( near_trace<0 || !useTrace(near_trace)) continue;       // no usable near trace found, we are done

        float prev_time=m_picks->value( traceIline(near_trace), traceXline(near_trace));
        traceTime = prev_time;
        traceTime=pick1(traceNo, traceTime, true);

        if( traceTime == NO_PICK) break;
    }

    emit picksChanged();
}


void Picker::pickFillRightNext(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    std::cout<<"2"<<std::endl<<std::flush;

    if( !useTrace(firstTraceNo)) return;        // first trace is zero trace, no pick here

    // pick first trace
    traceTime=pick1(firstTraceNo, traceTime);

    for( int traceNo = firstTraceNo+1; traceNo<m_gather->size(); traceNo++){

        if( !useTrace(traceNo)) continue;       // skip over zero traces

        traceTime=pick1(traceNo, traceTime); //, false);

        if( traceTime == NO_PICK) break;
    }

    emit picksChanged();
}


void Picker::pickFillLeftNearest(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    if( !useTrace(firstTraceNo)) return;        // first trace is zero trace, no pick here

    // pick first trace
    pick1(firstTraceNo, traceTime);

    for( int traceNo = firstTraceNo-1; traceNo>=0; traceNo--){

        if( !useTrace(traceNo)) continue;       // skip over zero traces

        int iline = traceIline(traceNo);
        int xline = traceXline(traceNo);

        // search for nearest non-zero trace, only use next traces in gather since they have been already picked
        int near_trace = -1;
        int near_dist2 = std::numeric_limits<int>::max();
        for( int i = traceNo + 1; i < m_gather->size(); i++){

            if( !useTrace(i)) continue;     // skip over zero trace

            int iline2 = traceIline(i);
            int xline2 = traceXline(i);
            if( !m_picks->contains(iline2, xline2)) continue;       // no pick for this trace, skip it
            int dist2 = SQR(iline2 - iline ) + SQR(xline2 - xline);
            if( dist2 < near_dist2 ){
                near_trace = i;
                near_dist2 = dist2;
            }
        }

        if( near_trace<0 || !useTrace(near_trace)) continue;       // no usable near trace found, we are done

        float prev_time=m_picks->value( traceIline(near_trace), traceXline(near_trace));
        traceTime = prev_time;
        traceTime=pick1(traceNo, traceTime, true);

        if( traceTime == NO_PICK) break;
    }

    emit picksChanged();
}



void Picker::pickFillLeftNext(int firstTraceNo, float traceTime){

    if( !m_gather || !m_picks || firstTraceNo<0 ) return;

    if( !useTrace(firstTraceNo)) return;        // first trace is zero trace, no pick here

    // pick first trace
    traceTime=pick1(firstTraceNo, traceTime);

    for( int traceNo = firstTraceNo-1; traceNo>=0; traceNo--){

        if( !useTrace(traceNo)) continue;       // skip over zero traces

        traceTime=pick1(traceNo, traceTime);//, false);

        if( traceTime == NO_PICK) break;
    }

    emit picksChanged();
}

void Picker::deletePick( int traceNo, float t ){

    //check this only here in external interface
    if( traceNo<0 || traceNo>=static_cast<int>(m_gather->size() ) ) return;

    deletePickFunc( traceNo, t );
}

bool Picker::delete1( int traceNo, float secs){

    if( traceNo<0 || traceNo>=static_cast<int>(m_gather->size() ) )
        throw std::runtime_error("delete1 called with invalid traceno");

    const seismic::Trace& trace=(*m_gather)[traceNo];
    int iline=traceIline(traceNo);
    int xline=traceXline(traceNo);

    // nothing to delete, allready NULL, stop here in conservative mode
    if( m_conservative && !m_picks->contains(iline, xline) ) return false;

    m_dirty=true;

    m_picks->remove(iline,xline);

    return true;
}

void Picker::deleteSingle( int traceNo, float secs ){

    if( !m_gather || !m_picks || traceNo<0 || traceNo>=m_gather->size() ) return;

    delete1(traceNo, secs);
    emit picksChanged();
}

void Picker::deleteBuffered( int traceNo, float secs ){

    if( m_bufferedPoints.size()>0){

        int inear=0;
        int mindist=std::numeric_limits<int>::max();
        for(int i=0; i<m_bufferedPoints.size(); i++){
            auto dist=std::abs(m_bufferedPoints[i].first-traceNo);
            if( dist<mindist){
                mindist=dist;
                inear=i;
            }
        }
        m_bufferedPoints.erase(m_bufferedPoints.begin()+inear);
        emit bufferedPointsChanged();
    }
    else{
        delete1(traceNo, secs);
        emit picksChanged();
    }
}

void Picker::deleteLeft( int traceNo, float secs ){

    if( !m_gather || !m_picks || traceNo<0 ) return;

    for( ; traceNo>=0; traceNo-- ){

        if( !delete1(traceNo, secs) ) break;
    }

    emit picksChanged();
}

void Picker::deleteRight( int traceNo, float secs ){

    if( !m_gather || !m_picks || traceNo<0 ) return;

    for( ; traceNo<m_gather->size(); traceNo++ ){

        if( !delete1(traceNo,secs) ) break;
    }

    emit picksChanged();
}

float Picker::adjustPick(const seismic::Trace & trace, float t){

    return adjustFunc(trace, t);
}



//* ORIGINAL
  float Picker::adjustMinimum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = trace.time_to_index(t); // t - trace.ft() ) / trace.dt();

    // move up
    int iup=it;
    while( (iup-1>=0) && (sam[iup-1]<sam[iup]) ) iup--;

    // move down
    int idown=it;
    while( (idown+1<sam.size()) && (sam[idown+1]<sam[idown]) ) idown++;

    if( iup == idown ) it=iup;      // we are already there, it==iup==idown
    else if( iup==it ) it=idown;    // only down movement
    else if( idown==it) it=iup;     // only up movement
    return trace.ft() + it * trace.dt();
}

// ORIGINAL
float Picker::adjustMaximum(const seismic::Trace & trace, float t){

    if( t<trace.ft() || t>trace.lt() ) return t;

    const seismic::Trace::Samples& sam = trace.samples();

    // sample index for t
    int it = trace.time_to_index(t); // t - trace.ft() ) / trace.dt();

    // move up
    int iup=it;
    while( (iup-1>=0) && (sam[iup-1]>sam[iup]) ) iup--;

    // move down
    int idown=it;
    while( (idown+1<sam.size()) && (sam[idown+1]>sam[idown]) ) idown++;

    if( iup == idown ) it=iup;      // we are already there, it==iup==idown
    else if( iup==it ) it=idown;    // only down movement
    else if( idown==it) it=iup;     // only up movement
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

    m_ilxluseBuffer.clear();   // check if this resizes, we don't want resize

    if( !m_gather ){
        return;
    }

    m_ilxluseBuffer.reserve(m_gather->size());
    for( int traceNo = 0; traceNo<m_gather->size(); traceNo++){
        const seismic::Trace& trace=(*m_gather)[traceNo];
        const seismic::Header& header=trace.header();
        if(header.find("iline")==header.end()) throw std::runtime_error("trace header has no iline key");
        int iline=header.at("iline").intValue();
        if(header.find("xline")==header.end()) throw std::runtime_error("trace header has no xline key");
        int xline=header.at("xline").intValue();
        if(header.find("trid")==header.end()) throw std::runtime_error("trace header has no trid");
        int trid=header.at("trid").intValue();
        bool use=(trid=2);     // no dead trace
        m_ilxluseBuffer.push_back(std::make_tuple(iline,xline,use));
    }

}

void Picker::updateModeFuncs(){

    switch( m_mode){

    case PickMode::Single:
        pickFunc = std::bind(&Picker::pickSingle, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteSingle, this, _1, _2 );
        break;

    case PickMode::Left:
        switch(m_fillMode){
        case PickFillMode::Next: pickFunc = std::bind(&Picker::pickFillLeftNext, this, _1, _2); break;
        case PickFillMode::Nearest: pickFunc = std::bind(&Picker::pickFillLeftNearest, this, _1, _2); break;
        default: qFatal("Unhandled PickFillMode"); break;
        }

        deletePickFunc = std::bind(&Picker::deleteLeft, this, _1, _2 );
        break;

    case PickMode::Right:
        switch(m_fillMode){
        case PickFillMode::Next: pickFunc = std::bind(&Picker::pickFillRightNext, this, _1, _2); break;
        case PickFillMode::Nearest: pickFunc = std::bind(&Picker::pickFillRightNearest, this, _1, _2); break;
        default: qFatal("Unhandled PickFillMode"); break;
        }

        deletePickFunc = std::bind(&Picker::deleteRight, this, _1, _2);
        break;

    case PickMode::Lines:
        pickFunc = std::bind(&Picker::pickLines, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteBuffered, this, _1, _2 );
        break;

    case PickMode::Outline:
        pickFunc = std::bind(&Picker::pickOutline, this, _1, _2);
        deletePickFunc = std::bind(&Picker::deleteBuffered, this, _1, _2 );
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
