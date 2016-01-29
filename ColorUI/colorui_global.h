#ifndef COLORUI_GLOBAL_H
#define COLORUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(COLORUI_LIBRARY)
#  define COLORUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define COLORUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // COLORUI_GLOBAL_H
