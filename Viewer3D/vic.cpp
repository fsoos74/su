#include "vic.h"

#include<QOpenGLBuffer>

VIC::VIC(): m_arrayBuf(QOpenGLBuffer::VertexBuffer), m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_arrayBuf.create();
    m_indexBuf.create();
    m_indexCount=0;
}


VIC::~VIC(){

    m_arrayBuf.destroy();
    m_indexBuf.destroy();
}


void VIC::setVertices(QVector<VIC::Vertex> vertices){

    m_arrayBuf.bind();
    m_arrayBuf.allocate(vertices.data(), vertices.size() * sizeof(VIC::Vertex));
    m_arrayBuf.release();
}

void VIC::setIndices(QVector<VIC::Index> indices){

    m_indexBuf.bind();
    m_indexBuf.allocate(indices.data(), indices.size() * sizeof(VIC::Index));
    m_indexBuf.release();

    m_indexCount=indices.size();
}

void VIC::setMode(GLenum m){

    m_mode=m;
}


VIC* VIC::makeVIC( QVector<Vertex> vertices, QVector<Index> indices, GLenum mode ){
    VIC* vic=new VIC;
    vic->setVertices(vertices);
    vic->setIndices(indices);
    vic->setMode(mode);
    return vic;
}
