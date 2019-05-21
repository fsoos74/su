#ifndef GRID1D_H
#define GRID1D_H

#include<vector>
#include<limits>
#include<stdexcept>
#include<cassert>

class Grid1DBounds{
public:

    Grid1DBounds():m_i1(0), m_i2(0){

    }

    Grid1DBounds( int i1, int i2):m_i1(i1), m_i2(i2){
        if( m_i1>m_i2) std::swap( m_i1, m_i2);
    }

    int i1()const{
        return m_i1;
    }

    int i2()const{
        return m_i2;
    }

    int ni()const{
        return 1+ m_i2 - m_i1;
    }

    bool isInside(int i)const{
        return ( i>=m_i1 && i<=m_i2);
    }

    bool operator==(const Grid1DBounds& other)const{
        return (m_i1==other.m_i1) && (m_i2==other.m_i2);
    }

    bool operator!=(const Grid1DBounds& other)const{
        return !(*this==other);
    }

private:
    int m_i1;
    int m_i2;
};


template<typename T>
class Grid1D
{
public:

    typedef T value_type;
    typedef int index_type;

    static const value_type NULL_VALUE;//= std::numeric_limits<value_type>::max();
    using bounds_type=Grid1DBounds;
    using values_type=std::vector<value_type>;

    typedef typename values_type::iterator iterator;
    typedef typename values_type::const_iterator const_iterator;

    Grid1D()=default;

    Grid1D( const Grid1D& other)=default;

    Grid1D( const bounds_type& bounds, const value_type& ival=NULL_VALUE)
        :m_bounds(bounds),
         m_values( bounds.ni(), ival)
    {
    }

    const bounds_type& bounds()const{
        return m_bounds;
    }

    value_type& operator()(const index_type& i){
        return m_values[i-m_bounds.i1()];
    }

    const value_type& operator()(const index_type& i)const{
        return m_values[i-m_bounds.i1()];
    }

    //this also checks if i,j is inside and returns NULL if not
    value_type valueAt( const index_type& i)const{

        if( m_bounds.isInside(i) ){
            return (*this)(i);
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

private:

    bounds_type  m_bounds;
    values_type  m_values;

};

template<typename T>
inline std::pair<T, T> valueRange( const Grid1D<T>& grid){

    float min=std::numeric_limits<T>::max();
    float max=std::numeric_limits<T>::lowest();

    for( auto /*typename Grid1D<T>::values_type::const_iterator*/ it=grid.values().cbegin(); it!=grid.values().cend(); ++it){

        if(*it==grid.NULL_VALUE) continue;

        if( *it<min ){
            min=*it;
        }
        if( *it>max ){
            max=*it;
        }
    }

    if( max<min){
        max=grid.NULL_VALUE;
        min=grid.NULL_VALUE;
    }

    return std::pair<float, float>(min,max);
}


#endif // GRID1D_H
