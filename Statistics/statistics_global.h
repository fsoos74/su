#ifndef STATISTICS_GLOBAL_H
#define STATISTICS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STATISTICS_LIBRARY)
#  define STATISTICSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define STATISTICSSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define STATISTICSSHARED_EXPOR
#endif

#endif // STATISTICS_GLOBAL_H
