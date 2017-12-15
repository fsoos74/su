#ifndef GRID_GLOBAL_H
#define GRID_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRID_LIBRARY)
#  define GRIDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GRIDSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define GRIDSHARED_EXPORT
#endif

#endif // GRID_GLOBAL_H
