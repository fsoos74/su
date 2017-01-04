#include "renderscene.h"


#include<vic.h>
#include<vit.h>


RenderScene::RenderScene()
{

}

RenderScene::~RenderScene(){

    foreach ( RenderItem* item, m_items) {
       delete item;
    }
}


void RenderScene::clear(){

    foreach ( RenderItem* item, m_items) {
       delete item;
    }

    m_items.clear();
}

// take ownership of item, i.e. resposible for deleting it
int RenderScene::add(RenderItem* item){

    if( !item ) return -1;

    int key=m_nextKey++;

    m_items.insert( key, item );

    return key;
}

