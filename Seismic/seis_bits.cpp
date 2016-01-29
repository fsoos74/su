#include "seis_bits.h"

#include<cstring>   //memcpy
#include<utility>   //swap

namespace seismic{

void swap_bytes(int16_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+1) );
}

void swap_bytes(int32_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+3) );
    std::swap( *(p+1), *(p+2) );
}

void swap_bytes(int64_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+7) );
    std::swap( *(p+1), *(p+6) );
    std::swap( *(p+2), *(p+5) );
    std::swap( *(p+3), *(p+4) );
}

void swap_bytes(uint16_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+1) );
}

void swap_bytes(uint32_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+3) );
    std::swap( *(p+1), *(p+2) );
}

void swap_bytes(uint64_t* pv ){
    char* p=reinterpret_cast<char*>(pv);
    std::swap( *p, *(p+7) );
    std::swap( *(p+1), *(p+6) );
    std::swap( *(p+2), *(p+5) );
    std::swap( *(p+3), *(p+4) );
}


void get_from_raw( int16_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(int16_t) );
    if( swap ) swap_bytes( dst );
}

void get_from_raw( int32_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(int32_t) );
    if( swap ) swap_bytes( dst );
}

void get_from_raw( int64_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(int64_t) );
    if( swap ) swap_bytes( dst );
}

void get_from_raw( uint16_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(uint16_t) );
    if( swap ) swap_bytes( dst );
}

void get_from_raw( uint32_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(uint32_t) );
    if( swap ) swap_bytes( dst );
}

void get_from_raw( uint64_t* dst, char* praw, bool swap){
    std::memcpy( reinterpret_cast<char*>(dst), praw, sizeof(uint64_t) );
    if( swap ) swap_bytes( dst );
}


void put_to_raw( int16_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(int16_t) );
    if( swap ) swap_bytes( reinterpret_cast<int16_t*>(praw) );
}

void put_to_raw( int32_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(int32_t) );
    if( swap ) swap_bytes( reinterpret_cast<int32_t*>(praw) );
}

void put_to_raw( int64_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(int64_t) );
    if( swap ) swap_bytes( reinterpret_cast<int64_t*>(praw) );
}

void put_to_raw( uint16_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(uint16_t) );
    if( swap ) swap_bytes( reinterpret_cast<uint16_t*>(praw) );
}

void put_to_raw( uint32_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(uint32_t) );
    if( swap ) swap_bytes( reinterpret_cast<uint32_t*>(praw) );
}

void put_to_raw( uint64_t* src, char* praw, bool swap){
    std::memcpy( praw, reinterpret_cast<char*>(src), sizeof(uint64_t) );
    if( swap ) swap_bytes( reinterpret_cast<uint64_t*>(praw) );
}



}

