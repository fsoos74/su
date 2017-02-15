#ifndef GRID3D_H
#define GRID3D_H

#include<array3d.h>
#include<algorithm>
#include<stdexcept>

class Grid3DBounds{
public:

    Grid3DBounds()
        :m_inline1(0), m_inline2(0),
          m_crossline1(0), m_crossline2(0),
          m_sampleCount(0), m_ft(0), m_dt(1){

    }

    Grid3DBounds( int inline1, int inline2,
                  int crossline1, int crossline2,
                  int sampleCount, double ft, double dt)
        :m_inline1(inline1), m_inline2(inline2),
          m_crossline1(crossline1), m_crossline2(crossline2),
          m_sampleCount(sampleCount), m_ft(ft), m_dt(dt){

        if(m_inline1>m_inline2) std::swap(m_inline1, m_inline2);
        if(m_crossline1>m_crossline2) std::swap(m_crossline1, m_crossline2);
        if(m_dt<=0) throw std::runtime_error("Attempted Grid3DBounds with dt<=0!");
    }

    int inline1()const{
        return m_inline1;
    }

    int inline2()const{
        return m_inline2;
    }

    int crossline1()const{
        return m_crossline1;
    }

    int crossline2()const{
        return m_crossline2;
    }

    int sampleCount()const{

        return m_sampleCount;
    }

    int inlineCount()const{

        return m_inline2-m_inline1+1;
    }

    int crosslineCount()const{

        return m_crossline2-m_crossline1+1;
    }

    double ft()const{   //time or depth of first sample
        return m_ft;
    }

    double dt()const{   //sampling interval
        return m_dt;
    }

    double lt()const{// time of last sample
        return m_ft + (m_sampleCount-1)*m_dt;   // fence post
    }

    bool isInside(int iline, int xline)const{
        return ( iline>=m_inline1&&iline<=m_inline2 && xline>=m_crossline1&&xline<=m_crossline2);
    }

    bool isInside(int iline, int xline, int sample)const{
        return ( iline>=m_inline1&&iline<=m_inline2 &&
                 xline>=m_crossline1&&xline<=m_crossline2 &&
                 sample>=0 && sample<=m_sampleCount);
    }

    bool isInside(int iline, int xline, double t)const{
        return ( iline>=m_inline1&&iline<=m_inline2 &&
                 xline>=m_crossline1&&xline<=m_crossline2 &&
                 t>=m_ft && t<=lt());
    }


    bool operator==(const Grid3DBounds& other)const{
        return (m_inline1==other.m_inline1 && m_inline2==other.m_inline2 &&
                m_crossline1==other.m_crossline1 && m_crossline2==other.m_crossline2 &&
                m_sampleCount==other.m_sampleCount && m_ft==other.m_ft && m_dt==other.m_dt);
    }

    bool operator!=(const Grid3DBounds& other)const{
        return !(*this==other);
    }

    double sampleToTime( int i )const{
        return m_ft + i*m_dt;
    }

    int timeToSample( double t )const{
        return (t-m_ft)/m_dt;
    }

private:



    int m_inline1;
    int m_inline2;
    int m_crossline1;
    int m_crossline2;
    int m_sampleCount;
    double m_ft;
    double m_dt;
};


template<typename T>
class Grid3D
{
public:


    typedef T value_type;
    typedef int index_type;
    static const value_type NULL_VALUE;// = std::numeric_limits<value_type>::max();
    using bounds_type=Grid3DBounds;
    using values_type=util::Array3D<value_type>;
    typedef typename values_type::iterator iterator;
    typedef typename values_type::const_iterator const_iterator;

    Grid3D(){}

    Grid3D( const Grid3D& other)=default;

    Grid3D( const bounds_type& bounds, const value_type& ival=NULL_VALUE)
        :m_bounds(bounds),
         m_values(bounds.inlineCount(), bounds.crosslineCount(), bounds.sampleCount(), ival)
    {
    }

    const bounds_type& bounds()const{
        return m_bounds;
    }

    value_type& operator()( index_type iline, index_type xline, index_type sample){
        //std::assert( m_bounds.inside(i,j));
        return m_values(static_cast<size_t>(iline-m_bounds.inline1()),
                        static_cast<size_t>(xline-m_bounds.crossline1()),
                        sample);
    }

    const value_type& operator()( index_type iline, index_type xline, index_type sample)const{
        //std::assert( m_bounds.inside(i,j));
        return m_values(static_cast<size_t>(iline-m_bounds.inline1()),
                        static_cast<size_t>(xline-m_bounds.crossline1()),
                        sample);
    }

    const values_type& values()const{
        return m_values;
    }

    std::pair< value_type, value_type> valueRange()const{

        // cannot use std::algorithm because have to honor NULL values
        value_type min=std::numeric_limits<value_type>::max();
        value_type max=std::numeric_limits<value_type>::lowest();
        for( typename values_type::const_iterator it=m_values.cbegin(); it!=m_values.cend(); ++it ){
            if( *it==NULL_VALUE) continue;
            if( *it < min ) min=*it;
            if( *it > max ) max=*it;
        }

        if( min>max ){
            return std::pair<value_type, value_type>(NULL_VALUE, NULL_VALUE);
        }

        return std::pair<value_type, value_type>(min, max);
    }

    value_type value( int iline, int xline, double t)const{

        if( !m_bounds.isInside(iline, xline ) ) return NULL_VALUE;

        int i=m_bounds.timeToSample(t);
        if( i<0 ) return NULL_VALUE;
        if( i>=m_bounds.sampleCount()) return NULL_VALUE;

        return (*this)(iline, xline, i);
    }

    size_t size()const{
        return m_values.size();
    }

    const value_type& operator[](size_t idx)const{
        return m_values[idx];
    }

    value_type& operator[](size_t idx){
        return m_values[idx];
    }

    iterator begin(){
        return m_values.begin();
    }

    iterator end(){
        return m_values.end();
    }

    const_iterator cbegin(){
        return m_values.cbegin();
    }

    const_iterator cend(){
        return m_values.cend();
    }

private:

    bounds_type  m_bounds;
    values_type  m_values;

};



#endif // GRID3D_H

