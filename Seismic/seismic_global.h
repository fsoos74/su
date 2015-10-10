#ifndef SEISMIC_GLOBAL_H
#define SEISMIC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SEISMIC_LIBRARY)
#  define SEISMICSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SEISMICSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SEISMIC_GLOBAL_H
