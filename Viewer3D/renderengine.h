#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#include<QVector>
#include<vertexdata.h>

class RenderEngine : protected QOpenGLFunctions
{
public:

    RenderEngine();
    virtual ~RenderEngine();

    void clear();
    void addVIT( QVector<VertexDataTexture> vertices, QVector<GLushort> indices, QImage tex, GLenum mode);
    void addVIC( QVector<VertexDataColor>, QVector<GLushort> indices, GLenum mode);

    void draw(QMatrix4x4 matrix);

private:

    // Vertex Index Texture
    struct VIT{
        QOpenGLBuffer   arrayBuf;
        QOpenGLBuffer   indexBuf;
        unsigned        indexCount=0;
        QOpenGLTexture* texture=0;
        GLenum          mode;
    };

    // Vertex Index Color
    struct VIC{
        QOpenGLBuffer   arrayBuf;
        QOpenGLBuffer   indexBuf;
        unsigned        indexCount=0;
        GLenum          mode;
    };

    void initShaders();

    void draw( QOpenGLShaderProgram*, VIT);
    void draw( QOpenGLShaderProgram*, VIC);

    QOpenGLShaderProgram m_textureProgram;
    QOpenGLShaderProgram m_colorProgram;

    QVector<VIT> m_vits;
    QVector<VIC> m_vics;
};

#endif // RENDERENGINE_H
