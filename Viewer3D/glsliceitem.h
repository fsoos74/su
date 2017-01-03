#ifndef GLSLICEITEM_H
#define GLSLICEITEM_H

#include<QVector>
#include<QImage>
#include<vertexdata.h>
#include<glsceneitem.h>
#include<grid3d.h>
#include<slicedef.h>
#include<memory>
#include<QOpenGLBuffer>
#include<QOpenGLTexture>
#include<colortable.h>

class GLSliceItem : public GLSceneItem
{
    Q_OBJECT

public:

    GLSliceItem(QObject* parent=0);

    std::shared_ptr<Grid3D<float>> volume()const{
        return m_volume;
    }

    SliceDef sliceDef()const{
        return m_sliceDef;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

public slots:

    void setSliceDef(SliceDef);

    void setVolume( std::shared_ptr<Grid3D<float>>);

protected:

    void updateGPU();
    void render(QOpenGLShaderProgram*);

private:

    void inlineSliceToGPU( int iline );
    void crosslineSliceToGPU( int xline );
    void sampleSliceToGPU( int sample );
    void makeVIT(QVector<VertexDataTexture> vertices, QVector<GLushort> indices, QImage image);

    std::shared_ptr<Grid3D<float>> m_volume;
    SliceDef m_sliceDef;

    ColorTable* m_colorTable;

    QOpenGLBuffer   arrayBuf;
    QOpenGLBuffer   indexBuf;
    unsigned        indexCount;
    QOpenGLTexture* texture;
};

#endif // GLSLICEITEM_H
