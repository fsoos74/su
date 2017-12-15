#ifndef ARRAY3D
#define ARRAY3D

#include<vector>
#include<array>


namespace util{

// hardcoded 3 dimensions instead of n because efficiency, need to proove this
template<typename T>
class Array3D{

public:

    using value_type=T;
    using container_type=std::vector< value_type>;
    using iterator=typename container_type::iterator;
    using const_iterator=typename container_type::const_iterator;
    using size_type=typename container_type::size_type;
    typedef std::array<size_type,3> index_type;

    Array3D():m_data(){}    // do not init m_sizes, msvc had problems, should be filled with 0 anyway

    Array3D( index_type sizes, const value_type& ivalue=value_type(0))
        :m_data(sizes[0]*sizes[1]*sizes[2], ivalue),
         m_sizes(sizes){

    }

    Array3D( size_type nx, size_type ny, size_type nz, const value_type& ivalue=value_type(0) ) :
        Array3D(index_type{nx,ny,nz}, ivalue){
    }

    size_type size()const{
    return m_data.size();
    }

    size_type size(size_type dimension)const{
    return m_sizes.at(dimension);
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
    value_type& operator[](const size_type& idx){
        return m_data[idx];
    }

    const value_type& operator[](const size_type& idx)const{
        return m_data[idx];
    }

    const value_type& at(const size_type& idx)const{
        return m_data.at(idx);
    }

    value_type& operator[](const index_type& idx){
        return m_data[flat_index(idx)];
    }

    const value_type& operator[](const index_type& idx)const{
        return m_data[flat_index(idx)];
    }

    const value_type& at(const index_type& idx)const{
        return m_data.at(flat_index(idx));
    }

    value_type& operator()( size_type ix, size_type iy, size_type iz){
    return m_data[flat_index(ix, iy, iz)];
    }

    const value_type& operator()( size_type ix, size_type iy, size_type iz)const{
    return m_data[flat_index(ix, iy, iz)];
    }

    const value_type& at( size_type ix, size_type iy, size_type iz)const{
    return m_data.at(flat_index(ix, iy, iz));
    }

private:

    size_type flat_index( size_type ix, size_type iy, size_type iz)const{
    return (ix*m_sizes[1]+iy)*m_sizes[2]+iz;
    }

    size_type flat_index( index_type idx)const{
        return (idx[0]*m_sizes[1]+idx[1])*m_sizes[2]+idx[2];
    }


    container_type m_data;
    std::array<size_type,3> m_sizes;
};



}

#endif // ARRAY3D

