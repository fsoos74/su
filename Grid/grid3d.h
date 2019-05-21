#ifndef GRID3D_H
#define GRID3D_H

#include<array3d.h>
#include<algorithm>
#include<stdexcept>
#include<grid1d.h>
#include<grid2d.h>
#include<memory>


class Grid3DBounds{
public:

    Grid3DBounds()
        :m_i1(0), m_i2(0),
          m_j1(0), m_j2(0),
          m_nt(0), m_ft(0), m_dt(1){

    }

    Grid3DBounds( int i1, int i2,
                  int j1, int j2,
                  int nt, double ft, double dt)
        :m_i1(i1), m_i2(i2),
          m_j1(j1), m_j2(j2),
          m_nt(nt), m_ft(ft), m_dt(dt){

        if(m_i1>m_i2) std::swap(m_i1, m_i2);
        if(m_j1>m_j2) std::swap(m_j1, m_j2);
        if(m_dt<=0) throw std::runtime_error("Attempted Grid3DBounds with dt<=0!");
    }

    int i1()const{
        return m_i1;
    }

    int i2()const{
        return m_i2;
    }

    int j1()const{
        return m_j1;
    }

    int j2()const{
        return m_j2;
    }

    int nt()const{

        return m_nt;
    }

    int ni()const{

        return m_i2-m_i1+1;
    }

    int nj()const{

        return m_j2-m_j1+1;
    }

    double ft()const{   //time or depth of first sample
        return m_ft;
    }

    double dt()const{   //sampling interval
        return m_dt;
    }

    double lt()const{// time of last sample
        return m_ft + (m_nt-1)*m_dt;   // fence post
    }

    bool isInside(int i, int j)const{
        return ( i>=m_i1&&i<=m_i2 && j>=m_j1&&j<=m_j2);
    }

    bool isInside(int i, int j, int sample)const{
        return ( i>=m_i1&&i<=m_i2 &&
                 j>=m_j1&&j<=m_j2 &&
                 sample>=0 && sample<=m_nt);
    }

    bool isInside(int i, int j, double t)const{
        return ( i>=m_i1&&i<=m_i2 &&
                 j>=m_j1&&j<=m_j2 &&
                 t>=m_ft && t<=lt());
    }


    bool operator==(const Grid3DBounds& other)const{
        return (m_i1==other.m_i1 && m_i2==other.m_i2 &&
                m_j1==other.m_j1 && m_j2==other.m_j2 &&
                m_nt==other.m_nt && m_ft==other.m_ft && m_dt==other.m_dt);
    }

    bool operator!=(const Grid3DBounds& other)const{
        return !(*this==other);
    }

    double sampleToTime( int i )const{
        return m_ft + i*m_dt;
    }

    // maybe add round for better accuracy
    int timeToSample( double t )const{
        return (t-m_ft)/m_dt;
    }

private:



    int m_i1;
    int m_i2;
    int m_j1;
    int m_j2;
    int m_nt;
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
         m_values(bounds.ni(), bounds.nj(), bounds.nt(), ival)
    {
    }

    const bounds_type& bounds()const{
        return m_bounds;
    }

    //this also checks if i,j is inside and returns NULL if not
    value_type valueAt( const index_type& i, const index_type& j, const index_type& k)const{

        if( m_bounds.isInside(i, j, k) ){
            return (*this)(i,j, k);
        }
        else{
            return NULL_VALUE;
        }
    }

    value_type& operator()( index_type i, index_type j, index_type sample){
        //std::assert( m_bounds.inside(i,j));
        return m_values(static_cast<size_t>(i-m_bounds.i1()),
                        static_cast<size_t>(j-m_bounds.j1()),
                        sample);
    }

    const value_type& operator()( index_type i, index_type j, index_type sample)const{
        //std::assert( m_bounds.inside(i,j));
        return m_values(static_cast<size_t>(i-m_bounds.i1()),
                        static_cast<size_t>(j-m_bounds.j1()),
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

        return std::pair<value_type, value_type>(min, max);
    }

    value_type value( int i, int j, double t)const{

        if( !m_bounds.isInside(i, j ) ) return NULL_VALUE;

        int k=m_bounds.timeToSample(t);
        if( k<0 ) return NULL_VALUE;
        if( k>=m_bounds.nt()) return NULL_VALUE;

        // linear interpolation between nearest samples
        auto v0 = (*this)(i, j, k);
        if( k+1==m_bounds.nt()) return v0;

        auto fraction=(t-m_bounds.sampleToTime(k))/m_bounds.dt();
        auto v1 = (*this)(i, j, k+1);

        if( !fraction) return v0;   // no interpolation

        if( v0==NULL_VALUE || v1==NULL_VALUE ) return NULL_VALUE; // interpolation between null values results in null value

        return (1. - fraction ) * v0 + fraction*v1; // interpolation
    }

    value_type* data(){
        return &m_values[0];
    }

    const value_type* data()const{
        return &m_values[0];
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


    double ft()const{
        return m_bounds.ft();
    }

    double dt()const{
        return m_bounds.dt();
    }

    void setFT( double ft){     // this is save becauseit does not change the number of samples
        m_bounds=Grid3DBounds(m_bounds.i1(), m_bounds.i2(), m_bounds.j1(), m_bounds.j2(),
                              m_bounds.nt(), ft, m_bounds.dt() );
    }

    void setDT( double dt){     // this is save becauseit does not change the number of samples
        m_bounds=Grid3DBounds(m_bounds.i1(), m_bounds.i2(), m_bounds.j1(), m_bounds.j2(),
                              m_bounds.nt(), m_bounds.ft(), dt );
    }
/*
    Grid2D<T> atK(int k)const{
        Grid2DBounds b2d(m_bounds.i1(), m_bounds.j1(), m_bounds.i2(), m_bounds.j2() );
        Grid2D<T> g2d(b2d);     // initialized with NULL_VALUE
        if( k<0 || k>=m_bounds.nt()) return g2d;
        for(int i=b2d.i1(); i<=b2d.i2(); i++){
            for( int j=b2d.j1(); j<=b2d.j2(); j++){
                auto v=(*this)(i,j,k);
                if( v!=NULL_VALUE) g2d(i,j)=v;
            }
        }
        return g2d;
    }

    bool setK(int k, const Grid2D<T>& g2d){
        if( k<0 || k>=m_bounds.nt()) return false;
        auto b2d=g2d.bounds();
        if( b2d.i1()!=m_bounds.i1() || b2d.i2()!=m_bounds.i2() || b2d.j1()!=m_bounds.j1() || b2d.j2()!=m_bounds.j2() ) return false;
        for(int i=b2d.i1(); i<=b2d.i2(); i++){
            for( int j=b2d.j1(); j<=b2d.j2(); j++){
                auto v=g2d(i,j);
                if( v!=g2d.NULL_VALUE) (*this)(i,j,k)=v;
            }
        }
        return true;
    }
*/
    // j->i, k->j
    std::unique_ptr<Grid2D<T>> atI(int i)const{
        if(i<m_bounds.i1() || i>m_bounds.i2() || m_bounds.nt()<1) return std::unique_ptr<Grid2D<T>>();
        Grid2DBounds b2d(m_bounds.j1(), 0, m_bounds.j2(), m_bounds.nt());
        auto g2d=std::make_unique<Grid2D<T>>(b2d);
        for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
            for(int k=0; k<m_bounds.nt(); k++){
                g2d(j,k)=(*this)(i,j,k);
            }
        }
        return g2d;
    }

    // i->i, k->j
    std::unique_ptr<Grid2D<T>> atJ(int j)const{
        if(j<m_bounds.j1() || j>m_bounds.j2() || m_bounds.nt()<1) return std::unique_ptr<Grid2D<T>>();
        Grid2DBounds b2d(m_bounds.i1(), 0, m_bounds.i2(), m_bounds.nt());
        auto g2d=std::make_unique<Grid2D<T>>(b2d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            for(int k=0; k<m_bounds.nt(); k++){
                g2d(j,k)=(*this)(i,j,k);
            }
        }
        return g2d;
    }

    // i->i, j->j
    std::unique_ptr<Grid2D<T>> atK(int k)const{
        if(k<0 || k>=m_bounds.nt()) return std::unique_ptr<Grid2D<T>>();
        Grid2DBounds b2d(m_bounds.i1(), m_bounds.j1(), m_bounds.i2(), m_bounds.j2());
        auto g2d=std::make_unique<Grid2D<T>>(b2d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
                g2d(i,j)=(*this)(i,j,k);
            }
        }
        return g2d;
    }

    // i->i, j->j
    std::unique_ptr<Grid2D<T>> atT(double t)const{
        if(t<m_bounds.ft() || t>m_bounds.lt()) return std::unique_ptr<Grid2D<T>>();
        Grid2DBounds b2d(m_bounds.i1(), m_bounds.j1(), m_bounds.i2(), m_bounds.j2());
        auto g2d=std::make_unique<Grid2D<T>>(b2d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
                (*g2d)(i,j)=value(i,j,t);
            }
        }
        return g2d;
    }

    std::unique_ptr<Grid1D<T>> atIJ(int i, int j)const{
        if(!m_bounds.isInside(i,j,0)) return std::unique_ptr<Grid1D<T>>();
        auto b1d=Grid1DBounds(0,m_bounds.nt()-1);
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int k=0; k<m_bounds.nt(); k++){
            (*g1d)(k)=(*this)(i,j,k);
        }
        return g1d;
    }

    std::unique_ptr<Grid1D<T>> atIK(int i, int k)const{
        if(!m_bounds.isInside(i,m_bounds.j1(),k)) return std::unique_ptr<Grid1D<T>>();
        auto b1d=Grid1DBounds(m_bounds.j1(),m_bounds.j2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
            (*g1d)(j)=(*this)(i,j,k);
        }
        return g1d;
    }

    std::unique_ptr<Grid1D<T>> atJK(int j, int k)const{
        if(!m_bounds.isInside(m_bounds.i1(),j,k)) return std::unique_ptr<Grid1D<T>>();
        auto b1d=Grid1DBounds(m_bounds.i1(),m_bounds.i2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            (*g1d)(i)=(*this)(i,j,k);
        }
        return g1d;
    }

    std::unique_ptr<Grid1D<T>> atIT(int i, double t)const{
        if(!m_bounds.isInside(i,m_bounds.j1(),t)) return std::unique_ptr<Grid1D<T>>();
        auto b1d=Grid1DBounds(m_bounds.j1(), m_bounds.j2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
            (*g1d)(j)=value(i,j,t);
        }
        return g1d;
    }

    std::unique_ptr<Grid1D<T>> atJT(int j, double t)const{
        if(!m_bounds.isInside(m_bounds.i1(),j,t)) return std::unique_ptr<Grid1D<T>>();
        auto b1d=Grid1DBounds(m_bounds.i1(), m_bounds.i2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            (*g1d)(j)=value(i,j,t);
        }
        return g1d;
    }

private:

    bounds_type  m_bounds;
    values_type  m_values;
};



#endif // GRID3D_H

