#ifndef PROZESS_GLOBAL_H
#define PROZESS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROZESS_LIBRARY)
#  define PROZESSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PROZESSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PROZESS_GLOBAL_H
