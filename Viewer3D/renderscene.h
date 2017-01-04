#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include<QList>
#include<QMap>
#include <renderitem.h>


class RenderScene
{
public:

    typedef int ItemID;

    RenderScene();
    ~RenderScene();

    void clear();
    ItemID addItem( RenderItem* item);
    bool removeItem( ItemID );
    inline RenderItem* getItem( ItemID );

    inline QList<RenderItem*> items()const;

private:

    int m_nextKey=1;

    QMap< ItemID, RenderItem*> m_items;
};

QList<RenderItem*> RenderScene::items()const{
    return m_items.values();
}

RenderItem* RenderScene::getItem( RenderScene::ItemID id ){
    return m_items.value( id, nullptr );
}



#endif // RENDERSCENE_H
