#ifndef GLSCENEITEM_H
#define GLSCENEITEM_H

#include<QObject>
#include<QOpenGLShaderProgram>
#include<QOpenGLFunctions>

class GLSceneItem : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    GLSceneItem(QObject* parent=0);

protected:

    virtual void updateGPU()=0;
    virtual void render(QOpenGLShaderProgram*)=0;
};

#endif // GLSCENEITEM_H
