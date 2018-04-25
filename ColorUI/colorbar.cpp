#include "colorbar.h"
#include<iostream>
#include<cmath>


using namespace std::placeholders;


ColorBar::ColorBar(int size, QObject *parent) : QObject(parent), m_entries(size, std::make_pair(m_nullValue, m_nullColor))
{
    updateMapFunc();
    updateLookup();
}

QRgb ColorBar::map(double value)const{
    return m_mapFunc(value);
}

void ColorBar::setMapMode(MapMode m){
    if( m==m_mapMode) return;
    m_mapMode=m;
    updateMapFunc();
    emit changed();
}

void ColorBar::setLocked(bool on){
    if( on==m_locked) return;
    m_locked=on;
    emit lockChanged(on);
}

void ColorBar::set(int i, double value, QRgb rgb){
    if( isLocked() ) return;
    set( i, std::make_pair(value, rgb));
}

void ColorBar::set(int i, std::pair<double,QRgb> e){
    Q_ASSERT(i>=0 && i<m_entries.size());
    if( e==m_entries[i]) return;
    m_entries[i]=e;
    updateLookup();
    emit changed();
}

void ColorBar::set(QVector<double> values, QVector<QRgb> colors){
    Q_ASSERT(values.size()==colors.size());
    if( isLocked() ) return;
    auto size=values.size();
    m_entries.resize(size);
    for( int i=0; i<size; i++){
        m_entries[i]=std::make_pair( values[i], colors[i]);
    }
    updateLookup();
    emit changed();
}

void ColorBar::setColor(int i, QRgb rgb){
    Q_ASSERT( i>=0 && i<m_entries.size());
    if( isLocked() ) return;
    if(m_entries[i].second==rgb) return;
    m_entries[i]=std::make_pair(m_entries[i].first, rgb);
    emit changed();
}

void ColorBar::setColors(QVector<QRgb> colors){
    Q_ASSERT(colors.size()==m_entries.size());
    if( isLocked() ) return;
    bool chng=false;
    for( int i=0; i<m_entries.size(); i++){
        if(colors[i]!=m_entries[i].second){
            m_entries[i]=std::make_pair(m_entries[i].first,colors[i]);
            chng=true;
        }
    }
    if(chng) emit changed();
}

void ColorBar::setValue(int i, double value){
    Q_ASSERT( i>=0 && i<m_entries.size());
    if( isLocked() ) return;
    if(m_entries[i].first==value) return;
    m_entries[i]=std::make_pair(value, m_entries[i].second);
    updateLookup();
    emit changed();
}

void ColorBar::setValues(QVector<double> values){
    Q_ASSERT(values.size()==m_entries.size());
    if( isLocked() ) return;
    bool chng=false;
    for( int i=0; i<m_entries.size(); i++){
        if(values[i]!=m_entries[i].first){
            m_entries[i]=std::make_pair(values[i], m_entries[i].second);
            chng=true;
        }
    }
    if(chng){
        updateLookup();
        emit changed();
    }
}

void ColorBar::setNullColor(QRgb rgb){
    if(rgb==m_nullColor) return;
    if( isLocked() ) return;
    m_nullColor=rgb;
    emit changed();
}

void ColorBar::setNullValue(double value){
    if( value==m_nullValue) return;
    if( isLocked() ) return;
    m_nullValue=value;
    emit changed();
}

void ColorBar::updateMapFunc(){
    switch(m_mapMode){
    case MapMode::Interpolate:
        m_mapFunc=std::bind(&ColorBar::mapInterpolated, this, _1);
        break;
    case MapMode::Nearest:
        m_mapFunc=std::bind(&ColorBar::mapNearest, this, _1);
        break;
    }
}

void ColorBar::updateLookup(){
    m_lookup.clear();
    for(int i=0; i<m_entries.size(); i++){
        auto const& entry=m_entries[i];
        if(entry.first!=m_nullValue){
            m_lookup.insert(entry.first, i);
        }
    }

    /*
    std::cout<<"LOOKUP:"<<std::endl;
    for( auto v : m_lookup.keys()){
        std::cout<<v<<" - "<<m_lookup.value(v)<<std::endl;
    }
    std::cout<<std::flush;
    */
}

QRgb ColorBar::mapInterpolated(double value)const{
    if(m_lookup.empty()) return m_nullColor;        // lookup contains only non-null values
    if(value==m_nullValue) return m_nullColor;
    auto itr=m_lookup.upperBound(value);            // lookup is sorted, first item>value
    if( itr==m_lookup.end()){                       // value greater than last (non-null) value
        //std::cout<<"last_key="<<m_lookup.lastKey()<<" last_val="<<m_lookup.last()<<std::endl<<std::flush;
        return m_entries[m_lookup.last()].second;
    }
    if( itr==m_lookup.begin()){                     // value less than first (non-null) value
        return m_entries[m_lookup.first()].second;
    }
    auto itl=itr-1;                                 // finally we have smaller and greater values

    auto l_entry=m_entries[*itl];
    auto l_value=l_entry.first;
    auto l_rgb=l_entry.second;
    auto l_r=qRed(l_rgb);
    auto l_g=qGreen(l_rgb);
    auto l_b=qBlue(l_rgb);
    auto l_a=qAlpha(l_rgb);

    auto r_entry=m_entries[*itr];
    auto r_value=r_entry.first;
    auto r_rgb=r_entry.second;
    auto r_r=qRed(r_rgb);
    auto r_g=qGreen(r_rgb);
    auto r_b=qBlue(r_rgb);
    auto r_a=qAlpha(r_rgb);

    if( l_value==r_value) return m_nullColor;

    auto fac=(value-l_value)/(r_value-l_value);
    auto r= (1.-fac)*l_r + fac*r_r;
    auto g= (1.-fac)*l_g + fac*r_g;
    auto b= (1.-fac)*l_b + fac*r_b;
    auto a= (1.-fac)*l_a + fac*r_a;

    return qRgba(r,g,b,a);
}

QRgb ColorBar::mapNearest(double value)const{
    if(m_lookup.empty()) return m_nullColor;        // lookup contains only non-null values
    if(value==m_nullValue) return m_nullColor;
    auto itr=m_lookup.upperBound(value);            // lookup is sorted, first item>value
    if( itr==m_lookup.end()){                       // value greater than last (non-null) value
        //std::cout<<"last_key="<<m_lookup.lastKey()<<" last_val="<<m_lookup.last()<<std::endl<<std::flush;
        return m_entries[m_lookup.last()].second;
    }
    if( itr==m_lookup.begin()){                     // value less than first (non-null) value
        return m_entries[m_lookup.first()].second;
    }
    auto itl=itr-1;                                 // finally we have smaller and greater values

    auto l_entry=m_entries[*itl];
    auto l_value=l_entry.first;
    auto l_rgb=l_entry.second;

    auto r_entry=m_entries[*itr];
    auto r_value=r_entry.first;
    auto r_rgb=r_entry.second;

    return (value-l_value<r_value-value) ? l_rgb : r_rgb;
}

QVector<double> ColorBar::linearValues(double first, double last, int n){

    if( n<1 ) return QVector<double>();
    if( first==last || n==1) return QVector<double>(1, (first+last)/2);

    QVector<double> values(n);
    for( int i=0; i<n; i++){
        values[i] = first + i * (last-first)/(n-1);
    }

    return values;
}

QVector<double> ColorBar::powerValues(double first, double last, double power, int n){

    if( n<1 ) return QVector<double>();
    if( first==last || n==1) return QVector<double>(1, (first+last)/2);

    QVector<double> values(n);
    for( int i=0; i<n; i++){
        auto value= first + std::pow( static_cast<double>(i)/(n-1), power)*(last-first)/(n-1);
        values[i] = std::pow( value, power);
    }

    return values;
}
