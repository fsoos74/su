#include "glsliceitem.h"

GLSliceItem::GLSliceItem(QObject* parent) : GLSceneItem(parent), m_colorTable(new ColorTable(this))
{
    m_colorTable->setColors(ColorTable::defaultColors());
}


void GLSliceItem::setSliceDef(SliceDef def){

    if( def==m_sliceDef) return;

    m_sliceDef=def;

    updateGPU();
}


void GLSliceItem::setVolume(std::shared_ptr<Grid3D<float> > v ){

    if( v==m_volume) return;

    m_volume=v;

    auto rg=m_volume->valueRange();
    m_colorTable->setRange(rg.first, rg.second);

    updateGPU();
}


void GLSliceItem::updateGPU(){

    if( !m_volume){

        indexCount=0;
        return;
    }

    switch( m_sliceDef.type){
    case SliceType::INLINE:
        inlineSliceToGPU(m_sliceDef.value);
        break;
    case SliceType::CROSSLINE:
        crosslineSliceToGPU(m_sliceDef.value);
        break;
    case SliceType::SAMPLE:
        sampleSliceToGPU(m_sliceDef.value);
        break;
    }

}


void GLSliceItem::render( QOpenGLShaderProgram* program ){

    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();
    texture->bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexDataTexture));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexDataTexture));



    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_SHORT, 0);
    //glDrawArrays(GL_TRIANGLE_STRIP,  0, vit.indexCount );

    texture->release();
    indexBuf.release();
    arrayBuf.release();
}


void GLSliceItem::inlineSliceToGPU( int iline ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    QImage img(bounds.crosslineCount(), bounds.sampleCount(), QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){
            img.setPixel(i,j,m_colorTable->map(volume(iline, bounds.crossline1()+i, j) ) );
        }
    }

    QVector<VertexDataTexture> vertices{
        {QVector3D( bounds.crossline1(), 0,  iline), QVector2D( 0, 0)},   // top left
        {QVector3D( bounds.crossline2(), 0, iline), QVector2D( 1, 0)},   // top right
        {QVector3D( bounds.crossline1(), bounds.sampleCount(), iline), QVector2D( 0, 1)},   // bottom left
        {QVector3D( bounds.crossline2(), bounds.sampleCount(), iline), QVector2D( 1, 1)}    // bottom right
    };

    QVector<GLushort> indices{
         0,  1,  2,  3
    };

    makeVIT(vertices, indices, img.mirrored());
}

void GLSliceItem::crosslineSliceToGPU( int xline ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    QImage img(bounds.inlineCount(), bounds.sampleCount(), QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){
            img.setPixel(i,j,m_colorTable->map(volume(bounds.inline1()+i, xline, j) ) );
        }
    }

    QVector<VertexDataTexture> vertices{
        {QVector3D( xline, 0,  bounds.inline1()), QVector2D( 0, 0)},   // top left
        {QVector3D( xline, 0, bounds.inline2()), QVector2D( 1, 0)},   // top right
        {QVector3D( xline, bounds.sampleCount(), bounds.inline1()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xline, bounds.sampleCount(), bounds.inline2()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<GLushort> indices{
         0,  1,  2,  3
    };

    makeVIT(vertices, indices, img.mirrored());
}

void GLSliceItem::sampleSliceToGPU( int sample ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    QImage img(bounds.inlineCount(), bounds.crosslineCount(), QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){
            img.setPixel(i,j,m_colorTable->map(volume(bounds.inline1()+i, bounds.crossline1()+j, sample) ) );
        }
    }

    QVector<VertexDataTexture> vertices{
        {QVector3D( bounds.crossline1(), sample,  bounds.inline1()), QVector2D( 0, 0)},   // top left
        {QVector3D( bounds.crossline1(), sample, bounds.inline2()), QVector2D( 1, 0)},   // top right
        {QVector3D( bounds.crossline2(), sample, bounds.inline1()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( bounds.crossline2(), sample, bounds.inline2()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<GLushort> indices{
         0,  1,  2,  3
    };

    makeVIT(vertices, indices, img.mirrored());
}

void GLSliceItem::makeVIT(QVector<VertexDataTexture> vertices, QVector<GLushort> indices, QImage image){

    QOpenGLBuffer abuf;
    abuf.create();
    abuf.bind();
    abuf.allocate(vertices.data(), vertices.size() * sizeof(VertexDataTexture));

    QOpenGLBuffer ibuf(QOpenGLBuffer::IndexBuffer);
    ibuf.create();
    ibuf.bind();
    ibuf.allocate(indices.data(), indices.size() * sizeof(GLushort));

    QOpenGLTexture* tex=new QOpenGLTexture(image);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    arrayBuf=abuf;
    indexBuf=ibuf;
    indexCount=indices.size();
    texture=tex;
}
