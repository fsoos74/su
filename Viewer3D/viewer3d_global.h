#ifndef VIEWER3D_GLOBAL_H
#define VIEWER3D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VIEWER3D_LIBRARY)
#  define VIEWER3DSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VIEWER3DSHARED_EXPORT Q_DECL_IMPORT
#endif


#ifdef STATICBUILD
#define VIEWER3DSHARED_EXPORT
#endif



#endif // VIEWER3D_GLOBAL_H
