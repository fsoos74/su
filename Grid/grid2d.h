#ifndef GRID2D_H
#define GRID2D_H

#include<vector>
#include<limits>
#include<stdexcept>
#include<cassert>
#include<array2d.h>
#include<rectangle.h>
#include<grid1d.h>
#include<memory>

class Grid2DBounds{
public:

    Grid2DBounds():m_i1(0), m_j1(0), m_i2(0), m_j2(0){

    }

    Grid2DBounds( int i1, int j1, int i2, int j2):m_i1(i1), m_j1(j1), m_i2(i2), m_j2(j2){
        if( m_i1>m_i2) std::swap( m_i1, m_i2);
        if( m_j1>m_j2) std::swap( m_j1, m_j2);
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

    int nj()const {
        return 1 + m_j2 - m_j1;
    }

    int ni()const{
        return 1+ m_i2 - m_i1;
    }

    int width()const {
        return nj();
    }

    int height()const{
        return ni();
    }

    bool isInside(int i, int j)const{
        return ( i>=m_i1 && i<=m_i2 && j>=m_j1 && j<=m_j2);
    }

    bool operator==(const Grid2DBounds& other)const{
        return (m_i1==other.m_i1) && (m_i2==other.m_i2) && (m_j1==other.m_j1) && (m_j2==other.m_j2);
    }

    bool operator!=(const Grid2DBounds& other)const{
        return !(*this==other);
    }

private:
    int m_i1;
    int m_j1;
    int m_i2;
    int m_j2;
};


template<typename T>
class Grid2D
{
public:


    typedef T value_type;
    typedef int index_type;

    static const value_type NULL_VALUE;// = std::numeric_limits<value_type>::max();
    using bounds_type=Grid2DBounds;
    using values_type=util::Array2D<value_type>;

    typedef typename values_type::iterator iterator;
    typedef typename values_type::const_iterator const_iterator;

    Grid2D()=default;

    Grid2D( const Grid2D& other)=default;

    Grid2D( const bounds_type& bounds, const value_type& ival=NULL_VALUE)
        :m_bounds(bounds),
         m_values( bounds.height(), bounds.width(), ival)
    {
    }

    const bounds_type& bounds()const{
        return m_bounds;
    }

    value_type& operator()(const index_type& i, const index_type& j){
        //std::assert( m_bounds.inside(i,j));
        return m_values(i-m_bounds.i1(), j-m_bounds.j1());
    }

    const value_type& operator()(const index_type& i, const index_type& j)const{
        //std::assert( m_bounds.inside(i,j) );
        return m_values(i-m_bounds.i1(), j-m_bounds.j1());
    }

    //this also checks if i,j is inside and returns NULL if not
    value_type valueAt( const index_type& i, const index_type& j)const{

        if( m_bounds.isInside(i,j) ){
            return (*this)(i,j);
        }
        else{
            return NULL_VALUE;
        }
    }


    const values_type& values()const{
        return m_values;
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

    value_type* data(){
        return &m_values[0];
    }

    size_t size()const{
        return m_values.size();
    }

    std::unique_ptr<Grid1D<T>> atI(int i)const{
        if(!m_bounds.isInside(i,m_bounds.j1())) return std::unique_ptr<Grid1D<T>>();
        Grid1DBounds b1d(m_bounds.j1(),m_bounds.j2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int j=m_bounds.j1(); j<=m_bounds.j2(); j++){
            (*g1d)(j)=(*this)(i,j);
        }
        return g1d;
    }

    std::unique_ptr<Grid1D<T>> atJ(int j)const{
        if(!m_bounds.isInside(m_bounds.i1(),j)) return std::unique_ptr<Grid1D<T>>();
        Grid1DBounds b1d(m_bounds.i1(),m_bounds.i2());
        auto g1d=std::make_unique<Grid1D<T>>(b1d);
        for(int i=m_bounds.i1(); i<=m_bounds.i2(); i++){
            (*g1d)(i)=(*this)(i,j);
        }
        return g1d;
    }

private:

    bounds_type  m_bounds;
    values_type  m_values;

};


std::pair<float, float> valueRange( const Grid2D<float>&);

#endif // GRID2D_H
