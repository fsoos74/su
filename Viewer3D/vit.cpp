#include "vit.h"

VIT::VIT():m_arrayBuf(QOpenGLBuffer::VertexBuffer), m_indexBuf(QOpenGLBuffer::IndexBuffer),
            m_texture( new QOpenGLTexture(QOpenGLTexture::Target2D) )
{
    m_arrayBuf.create();
    m_indexBuf.create();
    m_indexCount=0;

    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}


VIT::~VIT(){

    m_arrayBuf.destroy();
    m_indexBuf.destroy();
    delete m_texture;
}


void VIT::setVertices(QVector<VIT::Vertex> vertices){

    m_arrayBuf.bind();
    m_arrayBuf.allocate(vertices.data(), vertices.size() * sizeof(VIT::Vertex));
    m_arrayBuf.release();
}

void VIT::setIndices(QVector<VIT::Index> indices){

    m_indexBuf.bind();
    m_indexBuf.allocate(indices.data(), indices.size() * sizeof(VIT::Index));
    m_indexBuf.release();

    m_indexCount=indices.size();
}

void VIT::setMode(GLenum m){

    m_mode=m;
}

void VIT::setImage(QImage image){
    m_texture->setData(image);
}


VIT* VIT::makeVIT( QVector<VIT::Vertex> vertices, QVector<VIT::Index> indices,
                     GLenum mode, QImage image){
    VIT* vit=new VIT;

    vit->setVertices(vertices);
    vit->setIndices(indices);
    vit->setMode(mode);
    vit->setImage(image);

    return vit;
}

