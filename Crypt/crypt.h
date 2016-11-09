#ifndef CRYPT_H
#define CRYPT_H

#include "crypt_global.h"

#include<cstdlib>       // size_t

class CRYPTSHARED_EXPORT Crypt
{

public:
    Crypt();
};


namespace crypt{


CRYPTSHARED_EXPORT void encrypt( void*, size_t);

CRYPTSHARED_EXPORT void decrypt( void*, size_t);

}


#endif // CRYPT_H
