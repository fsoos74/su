#ifndef WELL_GLOBAL_H
#define WELL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WELL_LIBRARY)
#  define WELLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WELLSHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef STATICBUILD
#define WELLSHARED_EXPORT
#endif


#endif // WELL_GLOBAL_H
