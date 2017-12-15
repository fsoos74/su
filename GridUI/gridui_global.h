#ifndef GRIDUI_GLOBAL_H
#define GRIDUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRIDUI_LIBRARY)
#  define GRIDUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define GRIDUISHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define GRIDUISHARED_EXPORT
#endif


#endif // GRIDUI_GLOBAL_H
