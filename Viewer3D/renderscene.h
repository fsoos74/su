#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include<QList>
#include<QMap>
#include <renderitem.h>


class RenderScene
{
public:
    RenderScene();
    ~RenderScene();

    void clear();
    int add( RenderItem* item);

    inline QList<RenderItem*> items()const;

private:

    int m_nextKey=1;

    QMap< int, RenderItem*> m_items;
};

QList<RenderItem*> RenderScene::items()const{
    return m_items.values();
}




#endif // RENDERSCENE_H
