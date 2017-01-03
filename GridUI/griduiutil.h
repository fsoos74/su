#ifndef GRIDUIUTIL_H
#define GRIDUIUTIL_H

#include<grid2d.h>
#include "colortable.h"
#include<QImage>
#include<QColor>


QImage grid2image( const Grid2D<float>* grid, const ColorTable* colorTable, QColor nullColor=QColor(0,0,0,0) );

#endif // GRIDUIUTIL_H
