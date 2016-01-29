#ifndef SEIS_BITS_H
#define SEIS_BITS_H

#include<cstdint>

namespace seismic{

void swap_bytes(int16_t* pv );
void swap_bytes(int32_t* pv );
void swap_bytes(int64_t* pv );
void swap_bytes(uint16_t* pv );
void swap_bytes(uint32_t* pv );
void swap_bytes(uint64_t* pv );

void get_from_raw( int16_t* dst, char* praw, bool swap=false);
void get_from_raw( int32_t* dst, char* praw, bool swap=false);
void get_from_raw( int64_t* dst, char* praw, bool swap=false);
void get_from_raw( uint16_t* dst, char* praw, bool swap=false);
void get_from_raw( uint32_t* dst, char* praw, bool swap=false);
void get_from_raw( uint64_t* dst, char* praw, bool swap=false);

void put_to_raw( int16_t* src, char* praw, bool swap=false);
void put_to_raw( int32_t* src, char* praw, bool swap=false);
void put_to_raw( int64_t* src, char* praw, bool swap=false);
void put_to_raw( uint16_t* src, char* praw, bool swap=false);
void put_to_raw( uint32_t* src, char* praw, bool swap=false);
void put_to_raw( uint64_t* src, char* praw, bool swap=false);




/*
 *
 *
template<size_t n> void swap_bytes( char* p ){}

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

template<typename T> void put_to_raw( T* src, char* praw, bool swap=false){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(T) );
    if( swap ) swap_bytes< sizeof(T) >( praw );
}
*/

}



#endif // SEIS_BITS_H

