#ifndef GLSCENE_H
#define GLSCENE_H

#include<glsceneitem.h>
#include<QMap>

class GLScene
{
public:

    GLScene();

private:

    QMap<int, GLSceneItem*> m_items;
};

#endif // GLSCENE_H
