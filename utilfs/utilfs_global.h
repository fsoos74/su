#ifndef UTILFS_GLOBAL_H
#define UTILFS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILFS_LIBRARY)
#  define UTILFSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UTILFSSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define UTILFSSHARED_EXPORT
#endif

#endif // UTILFS_GLOBAL_H
