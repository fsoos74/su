#ifndef RENDERITEM_H
#define RENDERITEM_H

#include<QOpenGLFunctions>


class RenderItem : protected QOpenGLFunctions
{
public:
    RenderItem();
    virtual ~RenderItem();
};

#endif // RENDERITEM_H
