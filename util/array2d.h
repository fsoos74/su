#ifndef ARRAY2D_H
#define ARRAY2D_H


#include<vector>
#include<cassert>


namespace util{



// reference to part of 2d arays, e.g. rows or columns
template<typename T>
class SectionRef{

public:

    class iterator{

    public:

        iterator()=delete;

        iterator( SectionRef* sr, size_t pos ):m_section_ref(sr), m_pos(pos){}

        iterator& operator++(){++m_pos;return *this;}

        T& operator*(){
            return (*m_section_ref)[m_pos];
        }

        bool operator!=(const iterator& other)const{
            return m_section_ref!=other.m_section_ref || m_pos!=other.m_pos;
        }

    private:

        SectionRef* m_section_ref;
        size_t m_pos;

    };

    SectionRef()=delete;

    SectionRef( T* pdata, size_t incr, size_t count ):m_pdata(pdata), m_incr(incr), m_count(count){
    }

    size_t size()const{
        return m_count;
    }

    T& operator[](size_t idx){
        check_index(idx);
        return *(m_pdata + idx*m_incr);
    }

    iterator begin(){
        return iterator( this, 0 );
    }

    iterator end(){
        return iterator( this, m_count );
    }

private:

    void check_index(size_t idx){
        assert( idx<m_count );
    }

    T* m_pdata;
    size_t m_incr;
    size_t m_count;
};


template<typename T>
class Array2D
{

public:

    using value_type=T;
    using container_type=std::vector< value_type>;
    using iterator=typename container_type::iterator;
    using const_iterator=typename container_type::const_iterator;
    using size_type=typename container_type::size_type;

    Array2D()=default;

    Array2D( const Array2D& other)=default;

    Array2D( size_type rows, size_type cols, const value_type& ival=value_type(0) ): m_rows(rows), m_columns(cols), m_data(rows*cols, ival){
    }

    size_type size()const{
        return m_data.size();
    }

    size_type rows()const{
        return m_rows;
    }

    size_t columns()const{
        return m_columns;
    }

    iterator begin(){
        return m_data.begin();
    }

    iterator end(){
        return m_data.end();
    }

    const_iterator cbegin()const{
        return m_data.cbegin();
    }

    const_iterator cend()const{
        return m_data.cend();
    }

    const T& operator[](size_t idx)const{
        return m_data[idx];
    }

    T& operator[](size_t idx){
        return m_data[idx];
    }

    const T& operator()( size_t i, size_t j)const{
        return m_data[flat_index(i,j)];
    }

    T& operator()( size_t i, size_t j){
        return m_data[flat_index(i,j)];
    }

    T& at( size_type idx){
        return m_data.at(idx);
    }

    const T& at( size_type idx)const{
        return m_data.at(idx);
    }

    T& at( size_type i, size_type j){
        return m_data.at( flat_index(i,j));
    }

    const T& at( size_type i, size_type j)const{
        return m_data.at( flat_index(i,j));
    }



    SectionRef<T> row( size_t i ){
       return SectionRef<T>( &m_data[0] + i*m_columns, 1, m_columns );
    }

    SectionRef<T> column( size_t j ){
        return SectionRef<T>( &m_data[0] + j, m_columns, m_rows );
    }
/*
    void appendRows( const SimpleArray2d& other ){
        assert( m_columns==other.m_columns);
        SimpleArray2d tmp( m_rows + other.m_rows, m_columns );
        std::copy( m_data.begin(), m_data.end(), tmp.m_data.begin() );
        std::copy( other.m_data.begin(), other.m_data.end(), tmp.m_data.begin() + m_data.size() );
        std::swap( *this, tmp );
    }
*/
private:

    size_type flat_index( size_type i, size_type j )const{
        return i*m_columns + j;
    }

    size_type m_rows=0;
    size_type m_columns=0;
    container_type m_data;
};



}


#endif // ARRAY2D_H

