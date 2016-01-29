#ifndef UTILUI_GLOBAL_H
#define UTILUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILUI_LIBRARY)
#  define UTILUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define UTILUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILUI_GLOBAL_H
