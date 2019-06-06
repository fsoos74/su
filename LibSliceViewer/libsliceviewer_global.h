#ifndef LIBSLICEVIEWER_GLOBAL_H
#define LIBSLICEVIEWER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBSLICEVIEWER_LIBRARY)
#  define LIBSLICEVIEWERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBSLICEVIEWERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBSLICEVIEWER_GLOBAL_H
