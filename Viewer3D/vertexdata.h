#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include<QVector4D>
#include<QVector3D>
#include<QVector2D>

struct VertexDataTexture
{
    QVector3D position;
    QVector2D texCoord;
};


struct VertexDataColor
{
    QVector3D position;
    QVector3D color;
};


#endif // VERTEXDATA_H
