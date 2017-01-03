#include "griduiutil.h"

#include<iostream>

QImage grid2image( const Grid2D<float>* grid, const ColorTable* colorTable, QColor nullColor){

    if( !grid) return QImage();

    Q_ASSERT( colorTable);

    int n_colors=colorTable->colors().size();
    Q_ASSERT( n_colors>0);

    // in grid (i,j) i==row(y), j==column(x)
    // image i->x, j->y
    // width,height of bounds and image are reversed
    int imageWidth=grid->bounds().height();
    int imageHeight=grid->bounds().width();
    QImage img( imageWidth, imageHeight, QImage::Format_ARGB32);

    for( int x=0; x<imageWidth; x++ ){

        for( int y=0; y<imageHeight; y++){


            int i=grid->bounds().i1() + x;
            int j=grid->bounds().j1() + y;

            float v=(*grid)( i, j );
            QColor col;

            if( v==Grid2D<float>::NULL_VALUE){
                col=nullColor;
            }
            else{
                col=colorTable->map(v);
            }

            img.setPixel( x, y,  col.rgba());
        }

    }

    return img;
}
