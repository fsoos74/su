#include "volumeitem.h"

VolumeItem::VolumeItem(AVOProject* project, QGraphicsItem* parentGraphicsItem, QObject* parent):
    GridItem( project, parentGraphicsItem, parent)
{

}


void VolumeItem::setTime(double t){

    if( t==m_time) return;
    m_time=t;
    updateGrid();
}

void VolumeItem::setVolume(std::shared_ptr<Volume> v){

    if( v==m_volume ) return;
    m_volume=v;

    if( !v ) return;

    // create new grid according to bounds
    // IMPORTANT dont forget to handle all pointers to old grid, for instance colorbars!!!
    auto vbounds=v->bounds();
    Grid2DBounds bounds(vbounds.i1(), vbounds.j1(), vbounds.i2(), vbounds.j2());
    auto grid=std::make_shared<Grid2D<float>>(bounds);
    setGrid(grid);
    updateGrid();
}


void VolumeItem::updateGrid(){

    if( !m_volume) return;
    auto bounds=m_volume->bounds();

    auto g=grid();
    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            auto value = m_volume->value( i, j, m_time);
            if(value==m_volume->NULL_VALUE) value=g->NULL_VALUE;
            (*g)(i,j)=value;
        }
    }

    if( colorTable()) colorTable()->setRange(valueRange(*g));
    updatePixmap();
}
