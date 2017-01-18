#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVector>

#include<vic.h>
#include<vit.h>

class RenderScene;


class RenderEngine : protected QOpenGLFunctions
{
public:

    RenderEngine();
    virtual ~RenderEngine();

    void draw( RenderScene* scene, QMatrix4x4 matrix);

private:

    void initShaders();

    void draw( QOpenGLShaderProgram*, VIT*);
    void draw( QOpenGLShaderProgram*, VIC*);

    QOpenGLShaderProgram m_VICProgram;
    QOpenGLShaderProgram m_VITProgram;
};

#endif // RENDERENGINE_H
