#ifndef WELLUI_GLOBAL_H
#define WELLUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WELLUI_LIBRARY)
#  define WELLUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define WELLUISHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define WELLUISHARED_EXPORT
#endif


#endif // WELLUI_GLOBAL_H
