#ifndef VIT_H
#define VIT_H


#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <QImage>


// Vertex Index Texture
class VIT : protected QOpenGLFunctions
{
public:

    struct Vertex
    {
        QVector3D position;
        QVector2D texCoord;
    };

    typedef GLushort Index;

    VIT();
    ~VIT();

    inline QOpenGLBuffer arrayBuffer()const;
    inline QOpenGLBuffer indexBuffer()const;
    inline unsigned indexCount()const;
    inline GLenum mode()const;
    inline QOpenGLTexture* texture()const;

    void setVertices( QVector<Vertex> );
    void setIndices( QVector<Index> );
    void setMode( GLenum );
    void setImage( QImage );

    static VIT* makeVIT( QVector<Vertex> vertices, QVector<Index> indices,
                         GLenum mode, QImage image);

private:

    QOpenGLBuffer   m_arrayBuf;
    QOpenGLBuffer   m_indexBuf;
    unsigned        m_indexCount=0;
    GLenum          m_mode;
    QOpenGLTexture* m_texture=0;
};


inline QOpenGLBuffer VIT::arrayBuffer()const{
    return m_arrayBuf;
}

inline QOpenGLBuffer VIT::indexBuffer()const{
    return m_indexBuf;
}

inline unsigned VIT::indexCount()const{
    return m_indexCount;
}

inline GLenum VIT::mode()const{
    return m_mode;
}

inline QOpenGLTexture* VIT::texture()const{
    return m_texture;
}


#endif // VIT_H
