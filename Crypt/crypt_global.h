#ifndef CRYPT_GLOBAL_H
#define CRYPT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CRYPT_LIBRARY)
#  define CRYPTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CRYPTSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define CRYPTSHARED_EXPORT
#endif

#endif // CRYPT_GLOBAL_H
