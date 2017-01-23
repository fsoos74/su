#ifndef PIXMAPUTILS_H
#define PIXMAPUTILS_H


class QSize;
class QRect;
class QPixmap;

// scales src (virtually) to virtSize and copies copyRect
QPixmap virtualScaledCopy( QPixmap src, QSize virtSize, QRect copyRect );

// temporary scale and then copy
QPixmap scaledCopy( QPixmap src, QSize scaledSize, QRect copyRect );

#endif // PIXMAPUTILS_H
