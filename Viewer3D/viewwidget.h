#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>

#include "renderengine.h"


class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    RenderEngine* engine()const{
        return renderEngine;
    }

    QVector3D center(){
        return m_center;
    }

    QVector3D position(){
        return m_position;
    }

    QVector3D scale(){
        return m_scale;
    }

    QVector3D rotation(){
        return m_rotation;
    }

public slots:

    void setCenter( QVector3D );
    void setPosition( QVector3D );
    void setScale( QVector3D );
    void setRotation( QVector3D );

signals:

    void positionChanged(QVector3D);
    void scaleChanged(QVector3D);
    void rotationChanged(QVector3D);

protected:
    void mousePressEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:

    RenderEngine* renderEngine=nullptr;

    //QOpenGLShaderProgram textureProgram;
    //QOpenGLShaderProgram colorProgram;

    QMatrix4x4 projection;

    QVector3D m_center;
    QVector3D m_position;
    QVector3D m_rotation;
    QVector3D m_scale;
};

#endif // VIEWWIDGET_H
