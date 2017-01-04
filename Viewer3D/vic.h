#ifndef VIC_H
#define VIC_H


#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QVector>


// Vertex Index Color
class VIC : protected QOpenGLFunctions
{
public:

    struct Vertex
    {
        QVector3D position;
        QVector3D color;
    };

    typedef GLushort Index;

    VIC();
    ~VIC();

    inline QOpenGLBuffer arrayBuffer()const;
    inline QOpenGLBuffer indexBuffer()const;
    inline unsigned indexCount()const;
    inline GLenum mode()const;

    void setVertices( QVector<Vertex> );
    void setIndices( QVector<Index> );
    void setMode( GLenum );

    static VIC* makeVIC( QVector<Vertex> vertices, QVector<Index> indices, GLenum mode );

private:

    QOpenGLBuffer   m_arrayBuf;
    QOpenGLBuffer   m_indexBuf;
    unsigned        m_indexCount=0;
    GLenum          m_mode;

};


inline QOpenGLBuffer VIC::arrayBuffer()const{
    return m_arrayBuf;
}

inline QOpenGLBuffer VIC::indexBuffer()const{
    return m_indexBuf;
}

inline unsigned VIC::indexCount()const{
    return m_indexCount;
}

inline GLenum VIC::mode()const{
    return m_mode;
}

#endif // VIC_H
