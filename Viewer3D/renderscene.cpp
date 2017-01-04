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
RenderScene::ItemID RenderScene::addItem(RenderItem* item){

    if( !item ) return -1;

    ItemID key=m_nextKey++;

    m_items.insert( key, item );

    return key;
}

// deletes the item
bool RenderScene::removeItem( RenderScene::ItemID id){

    RenderItem* item=m_items.value( id, nullptr );

    if( ! item ) return false;

    delete item;

    m_items.remove(id);

    return true;
}
