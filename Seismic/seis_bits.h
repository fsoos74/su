#ifndef SEIS_BITS_H
#define SEIS_BITS_H

#include<cstring>   //memcpy

namespace seismic{

template<size_t n> void swap_bytes( char* p ){/*do nothing*/}

template<> void swap_bytes<2>(char* p){
    std::swap( *p, *(p+1) );
}

template<> void swap_bytes<4>(char* p){
    std::swap( *p, *(p+3) );
    std::swap( *(p+1), *(p+2) );
}

template<> void swap_bytes<8>(char* p){
    std::swap( *p, *(p+7) );
    std::swap( *(p+1), *(p+6) );
    std::swap( *(p+2), *(p+5) );
    std::swap( *(p+3), *(p+4) );
}



template<typename T> void get_from_raw( T* dst, char* praw, bool swap=false){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(T) );
    if( swap ) swap_bytes< sizeof(T) >( reinterpret_cast<char*>(dst) );
}



}



#endif // SEIS_BITS_H

