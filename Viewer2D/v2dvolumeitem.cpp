#include "v2dvolumeitem.h"

V2DVolumeItem::V2DVolumeItem(AVOProject* project, QGraphicsItem* parentGraphicsItem, QObject* parent):
    GridItem( project, parentGraphicsItem, parent)
{

}


void V2DVolumeItem::setSliceValue(int t){

    if( t==m_sliceValue) return;
    m_sliceValue=t;
    updateGrid();
}

void V2DVolumeItem::setVolume(std::shared_ptr<Volume> v){

    if( v==m_volume ) return;
    m_volume=v;
    updateGrid();
    /*
    if( !v ) return;


    // create new grid according to bounds
    // IMPORTANT dont forget to handle all pointers to old grid, for instance colorbars!!!
    auto vbounds=v->bounds();
    Grid2DBounds bounds(vbounds.i1(), vbounds.j1(), vbounds.i2(), vbounds.j2());
    auto grid=std::make_shared<Grid2D<float>>(bounds);
    setGrid(grid);
    updateGrid();
    */
}

void V2DVolumeItem::setZValueWrapper(int i){
    if(i==zValue()) return;
    setZValue(i);
}

void V2DVolumeItem::updateGrid(){
    if( !m_volume) return;
    auto bounds=m_volume->bounds();
    auto gbounds=Grid2DBounds(bounds.i1(),bounds.j1(),bounds.i2(),bounds.j2());
    auto g=std::make_shared<Grid2D<float>>(gbounds);
    for( int i=bounds.i1(); i<=bounds.i2(); i++){
        for( int j=bounds.j1(); j<=bounds.j2(); j++){
            auto value = m_volume->value( i, j, 0.001*m_sliceValue);      // msec -> sec
            if(value==m_volume->NULL_VALUE) value=g->NULL_VALUE;
            (*g)(i,j)=value;
        }
    }
    setGrid(g);
}
