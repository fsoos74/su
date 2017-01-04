#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include<QVector>

#include<vic.h>
#include<vit.h>



class RenderEngine : protected QOpenGLFunctions
{
public:

    RenderEngine();
    virtual ~RenderEngine();

    void clear();
    void addVIT( VIT* vit);
    void addVIC( VIC* vic);

    void draw(QMatrix4x4 matrix);

private:

    void initShaders();

    void draw( QOpenGLShaderProgram*, VIT*);
    void draw( QOpenGLShaderProgram*, VIC*);

    QOpenGLShaderProgram m_textureProgram;
    QOpenGLShaderProgram m_colorProgram;


    QVector<VIT*> m_vits;
    QVector<VIC*> m_vics;
};

#endif // RENDERENGINE_H
