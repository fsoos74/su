#include "axis.h"

#include<cmath>
#include<iostream>
#include<limits>


qreal Axis::NO_CURSOR=std::numeric_limits<qreal>::max();

Axis::Axis( Type type, qreal min, qreal max, QObject* parent)
    : QObject(parent), m_type(type), m_min(min), m_max(max), m_minView(min), m_maxView(max), m_minSelection(min), m_maxSelection(min){
    computeAutomaticInterval();
}

qreal Axis::toScene(const qreal& x)const{

    switch(m_type){
    case Type::Linear: return x;
        break;
    case Type::Logarithmic: return (x>1.e-10) ? std::log10(x):-10;	// XXX
        break;
    }
    return x;
}

qreal Axis::toAxis(const qreal& x)const{

    switch(m_type){
    case Type::Linear: return x;
        break;
    case Type::Logarithmic: return std::pow(10., x);
        break;
    }
    return x;
}

std::vector<Axis::Tick> Axis::computeTicks( qreal start, qreal stop )const{

    switch( m_type){
    case Type::Linear: return computeTicksLinear(start, stop);
        break;
    case Type::Logarithmic: return computeTicksLogarithmic(start, stop);
        break;
    default:  // should never happen
        return std::vector<Tick>();
    }
}


void Axis::setName(QString n){
    if( n==m_name) return;
    m_name=n;
    emit nameChanged(n);
}

void Axis::setUnit(QString u){
    if(u==m_unit) return;
    m_unit=u;
    emit unitChanged(u);
}

void Axis::setAutomaticInterval(bool on){

    if( on==m_automatic ) return;
    m_automatic = on;
    //if( on ) computeAutomaticInterval();  // dont do this from here !!!
}

void Axis::adjustRange(){

    if( m_type!=Type::Linear) return;

    if( m_interval<=0) return;

    auto amin=m_interval * std::floor( m_min / m_interval );  // tick less or equal
    auto amax=m_interval * std::ceil(m_max / m_interval );    // tick equal or greater
    // search minimum range with at least two ticks inside
    if( amin<=m_min && amax>=m_max){  //no tick in range

        setRange(amin,amax);
    }
    else if( m_min <= amin && m_max < amax ){ // tick missing on right side

        setRange(m_min, amax);
    }
    else if( amin < m_min && m_max >= amax ){ // tick missing on left side

        setRange(amin, m_max);
    }
    setViewRange(amin,amax);
}


void Axis::setRange(qreal min, qreal max){

    if( m_type==Type::Logarithmic){ // no log of 0 or negative numbers allowed
        if( min<=0 ) min=0.0000001;
        if( max<=0 ) max=0.0000001;
    }

    if(min>max) std::swap(min,max);

    if( min==m_min && max==m_max ) return;
    m_min=min;
    m_max=max;

    emit rangeChanged(m_min, m_max);

    if( m_automatic){
        computeAutomaticInterval();
    }
}

void Axis::setReversed(bool on){

    if( on==m_reverse) return;

    m_reverse=on;

    emit reversedChanged(on);
}

void Axis::setType( Type t){

    if( t==m_type) return;
    m_type=t;
    emit typeChanged(t);
    if( m_automatic ) computeAutomaticInterval();
}

void Axis::setInterval( qreal i){

    if( i==m_interval) return;
    if(!i) return;
    m_interval=i;

    emit ticksChanged(); //intervalsChanged( m_interval, m_nsub);
}

void Axis::setSubTickCount( int n ){

    if( n==m_nsub) return;
    m_nsub=n;
    emit ticksChanged();
}

void Axis::setLabelSubTicks(bool on){

    if( on == m_labelSubTicks) return;
    m_labelSubTicks = on;
    emit ticksChanged();
}

void Axis::setViewRange(qreal min, qreal max){

    if( min==m_minView && max==m_maxView ) return;
    if(min>max) std::swap(min,max);
    m_minView=min;
    m_maxView=max;

    emit viewRangeChanged(min, max);

    if(  m_automatic) computeAutomaticInterval();
}

void Axis::setLabelPrecision(int p){
    if( p==m_labelPrecision) return;
    m_labelPrecision=p;
    emit labelPrecisionChanged(p);
}

void Axis::setViewPixelLength(int l){
    if( l==m_viewPixelLength) return;

    m_viewPixelLength=l;
    if(m_automatic) computeAutomaticInterval();
    emit viewPixelLengthChanged(l);
}

void Axis::setSelection(qreal min, qreal max){
    if(min==m_minSelection && max==m_maxSelection) return;
    if(min>max) std::swap(min,max);
    m_minSelection=min;
    m_maxSelection=max;
    emit selectionChanged(min,max);
}

void Axis::setCursorPosition(qreal x){

    if( x==m_cursorPosition ) return;
    m_cursorPosition=x;
    emit cursorPositionChanged(x);
}

std::vector<Axis::Tick> Axis::computeTicksLinear( qreal start, qreal stop )const{

    std::vector<Tick> ticks;
    if( m_interval<=0 ) return ticks;

    if( start > stop ) std::swap(start, stop);

    long first=static_cast<long>(std::floor(start / m_interval ));
    long last=static_cast<long>(std::ceil( stop / m_interval ));

    ticks.reserve((last-first+2)*(m_nsub+1) );    // a little too much sometimes but better than not reserving
    qreal w=m_interval;
    qreal wsub=w/(m_nsub+1);

    for( long i=first; i<=last; i++ ){
        qreal t=i*w;
        QString lbl=QString::number(t, 'g', m_labelPrecision);
        ticks.push_back(Tick{Tick::Type::Main, t, lbl} );

        // sub ticks
        for( int j=1; j<=m_nsub; j++){
            qreal s=t + j*wsub;
            QString lbl;
            if( m_labelSubTicks ) lbl=QString::number(s);
            ticks.push_back(Tick{Tick::Type::Sub, s, lbl} );
        }
    }

    return ticks;
}

std::vector<Axis::Tick> Axis::computeTicksLogarithmic( qreal start, qreal stop )const{
    std::vector<Tick> ticks;

    if( stop<start ){
        std::swap( start, stop);
    }

    if( start<=0){
        qWarning("Start value of logarithmic scale must be greater than 0!!!");
        return ticks;
    }

    long i1=static_cast<long>( std::floor(std::log10(start) ) );
    long i2=static_cast<long>( std::ceil(std::log10(stop) ) );

    for( long i=i1; i<=i2; i++){
       double t=std::pow( 10., double(i) );
       QString lbl=QString::number(t, 'g', m_labelPrecision);
       ticks.push_back(Axis::Tick{Axis::Tick::Type::Main, t, lbl});

       for( int j=1; j<m_nsub; j++){
           double s=std::pow( 10., i+1) *(double(j+1)/(m_nsub+1));
           QString lbl;
           if( m_labelSubTicks) lbl=QString::number(s);
           ticks.push_back(Axis::Tick{Axis::Tick::Type::Sub, s, lbl});
       }

   }

    return ticks;
}

void Axis::computeAutomaticInterval(){
    const int MIN_TICK_DIST=50.;

    //if( m_minPixel==m_maxPixel) return;
    if( m_type==Axis::Type::Linear ){

        qreal pix_per_unit=std::fabs( qreal(m_viewPixelLength) / (m_maxView-m_minView) );
        qreal mag = std::pow( 10., std::ceil(std::log10(MIN_TICK_DIST/pix_per_unit)) );
        qreal incr;
        int stc;
        if( 0.1 * mag * pix_per_unit >= MIN_TICK_DIST){
                incr = 0.1*mag;
                stc=9;
        }
        else  if( 0.2 * mag * pix_per_unit >= MIN_TICK_DIST){
            incr = 0.2*mag;
            stc=1;
        }
        else  if( 0.5 * mag * pix_per_unit >= MIN_TICK_DIST){
            incr = 0.5*mag;
            stc=4;
        }
        else{
            incr=mag;
            stc=9;
        }


        setSubTickCount(stc);
        setInterval(incr);

    }
    else{   // logarithmic interval always power of 10
        //qreal pix_per_unit=std::fabs( qreal(m_viewPixelLength) / (m_maxView-m_minView) );
        //qreal mag = std::pow( 10., std::ceil(std::log10(MIN_TICK_DIST/pix_per_unit)) );
        setSubTickCount(9);
        setInterval(10);
    }
}
