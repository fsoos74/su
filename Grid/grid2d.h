#ifndef GRID2D_H
#define GRID2D_H

#include<vector>
#include<limits>
#include<stdexcept>
#include<cassert>
#include<array2d.h>
#include<rectangle.h>

class Grid2D
{
public:

    typedef double value_type;
    typedef int index_type;
    static const value_type NULL_VALUE;// = std::numeric_limits<value_type>::max();
    using bounds_type=util::Rectangle<index_type>;
    using values_type=util::Array2D<value_type>;

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
        return m_values(i-m_bounds.x1(), j-m_bounds.y1());
    }

    const value_type& operator()(const index_type& i, const index_type& j)const{
        //std::assert( m_bounds.inside(i,j) );
        return m_values(i-m_bounds.x1(), j-m_bounds.y1());
    }


private:

    bounds_type  m_bounds;
    values_type  m_values;

};



#endif // GRID2D_H
