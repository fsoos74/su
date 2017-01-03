#include "cryptfunc.h"

#include<vector>
#include<map>
#include<cstdint>
#include<cstdlib>
#include<cstring>



namespace crypt{

typedef uint16_t int_t;
const size_t BLOCK_SIZE=sizeof(int_t)*8;
const size_t BITS=BLOCK_SIZE;
const size_t BYTES=sizeof(int_t);


std::map<size_t, size_t> swap_map{

    { 12, 0}, {11,1}, {2,6}, {7,14}, {4,15}, {8,10}, {5, 13}, {9,3}
};


// dest, src, size=BLOCK_SIZE int_t
void crypt_block( int_t* dest, int_t* src){

    std::memset( dest, 0, BLOCK_SIZE*BYTES);

    for( size_t i=0; i<BLOCK_SIZE; i++ ){

        int_t imask=(1<<i);

        for( size_t j=0; j<BITS; j++){

            int_t omask=1<<j;
            if( src[j]&imask ){
                dest[i]|=omask;
            }
        }
    }

}


// in place
void shuffle_block( int_t* p ){

    for( auto m : swap_map ){
        std::swap( p[m.first], p[m.second] );
    }
}

void reverse_copy( void* dest, void* src, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(dest);
    uint8_t* _src=reinterpret_cast<uint8_t*>(src);
    for( size_t i=0; i<count; i++){
//std::cout<<"rc "<<i<<" / "<<count<<std::endl<<std::flush;
        _dest[i]=_src[count-1-i];
    }
}

void reverse_copy( void* dest, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(dest);
    for( size_t i=0; i<count/2; i++){
        std::swap( _dest[i], _dest[count-i-1] );
    }


}

void crypt( void* dest, void* src, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(dest);
    uint8_t* _src=reinterpret_cast<uint8_t*>(src);

    while( count>=BLOCK_SIZE*BYTES ){
        crypt_block( reinterpret_cast<int_t*>(_dest), reinterpret_cast<int_t*>(_src));
        _src+=BLOCK_SIZE*BYTES;
        _dest+=BLOCK_SIZE*BYTES;
        count-=BLOCK_SIZE*BYTES;
    }

    if( count>0 ){
        // if not enough left just copy
        reverse_copy( _dest, _src, count );
    }
}




void encrypt_block( void* dest, void* src){

    int_t* _dest=reinterpret_cast<int_t*>(dest);
    int_t* _src=reinterpret_cast<int_t*>(src);
    crypt_block( _dest, _src);
    shuffle_block(_dest);
}

void decrypt_block( void* dest, void* src){

    int_t* _dest=reinterpret_cast<int_t*>(dest);
    int_t* _src=reinterpret_cast<int_t*>(src);
    shuffle_block(_src);
    crypt_block( _dest, _src);
}




void encrypt( void* dest, void* src, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(dest);
    uint8_t* _src=reinterpret_cast<uint8_t*>(src);

    while( count>=BLOCK_SIZE*BYTES ){
        crypt_block( reinterpret_cast<int_t*>(_dest), reinterpret_cast<int_t*>(_src));
        shuffle_block(reinterpret_cast<int_t*>(_dest));
        _src+=BLOCK_SIZE*BYTES;
        _dest+=BLOCK_SIZE*BYTES;
        count-=BLOCK_SIZE*BYTES;

    }

    if( count>0 ){
        // if not enough left just copy
        reverse_copy( _dest, _src, count );
    }
}


void encrypt( void* p, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(p);

    int_t buf[BLOCK_SIZE];

    while( count>=BLOCK_SIZE*BYTES ){

        std::memcpy( &buf[0], _dest, BLOCK_SIZE*BYTES );
        crypt_block( reinterpret_cast<int_t*>(_dest), &buf[0]);
        shuffle_block(reinterpret_cast<int_t*>(_dest));
        _dest+=BLOCK_SIZE*BYTES;
        count-=BLOCK_SIZE*BYTES;
    }

    if( count>0 ){
        // if not enough left just copy
        reverse_copy( _dest, count );
    }
}

void decrypt( void* dest, void* src, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(dest);
    uint8_t* _src=reinterpret_cast<uint8_t*>(src);

    int_t buf[BLOCK_SIZE];

    while( count>=BLOCK_SIZE*BYTES ){

        std::memcpy( &buf[0], _src, BLOCK_SIZE*BYTES );
        shuffle_block(reinterpret_cast<int_t*>(&buf[0]));
        crypt_block( reinterpret_cast<int_t*>(_dest), &buf[0]);
        _src+=BLOCK_SIZE*BYTES;
        _dest+=BLOCK_SIZE*BYTES;
        count-=BLOCK_SIZE*BYTES;
    }

    if( count>0 ){
        // if not enough left just copy
        reverse_copy( _dest, _src, count );
    }
}


void decrypt( void* p, size_t count){

    uint8_t* _dest=reinterpret_cast<uint8_t*>(p);

    int_t buf[BLOCK_SIZE];

    while( count>=BLOCK_SIZE*BYTES ){

        std::memcpy( &buf[0], _dest, BLOCK_SIZE*BYTES );
        shuffle_block(&buf[0]);
        crypt_block( reinterpret_cast<int_t*>(_dest), &buf[0]);
        _dest+=BLOCK_SIZE*BYTES;
        count-=BLOCK_SIZE*BYTES;
    }

    if( count>0 ){
        // if not enough left just copy
        reverse_copy( _dest, count );
    }
}

}
