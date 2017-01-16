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
#include "renderscene.h"

class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    RenderEngine* engine()const{
        return m_engine;
    }

    RenderScene* scene()const{
        return m_scene;
    }

    QVector3D center()const{
        return m_center;
    }

    QVector3D position()const{
        return m_position;
    }

    QVector3D scale()const{
        return m_scale;
    }

    QVector3D rotation()const{
        return m_rotation;
    }

    QVector3D dimesions()const{
        return m_dimensions;
    }

    qreal relativeStepSize()const{
        return m_relativeStepSize;
    }

    qreal rotationStep()const{
        return m_rotationStep;
    }

    qreal scaleFactor()const{
        return m_scaleFactor;
    }

public slots:

    //void setScene( RenderScene* );
    void setCenter( QVector3D );
    void setPosition( QVector3D );
    void setScale( QVector3D );
    void setRotation( QVector3D );
    void setDimensions( QVector3D );

    void setRelativeStepSize( qreal );
    void moveXPos();
    void moveXNeg();
    void moveYPos();
    void moveYNeg();
    void moveZPos();
    void moveZNeg();

    void setRotationStep( qreal );
    void rotateXPos();
    void rotateXNeg();
    void rotateYPos();
    void rotateYNeg();
    void rotateZPos();
    void rotateZNeg();

    void setScaleFactor( qreal );
    void scaleXPos();
    void scaleXNeg();
    void scaleYPos();
    void scaleYNeg();
    void scaleZPos();
    void scaleZNeg();

signals:

    void positionChanged(QVector3D);
    void scaleChanged(QVector3D);
    void rotationChanged(QVector3D);
    void dimensionsChanged(QVector3D);

protected:
    void mousePressEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:

    RenderEngine* m_engine=nullptr;
    RenderScene* m_scene=nullptr;

    //QOpenGLShaderProgram textureProgram;
    //QOpenGLShaderProgram colorProgram;

    QMatrix4x4 projection;

    QVector3D m_center;
    QVector3D m_position;
    QVector3D m_rotation;
    QVector3D m_scale;
    QVector3D m_dimensions;      // this is not scaled yet, x and z are true coords, y is in secs

    qreal m_relativeStepSize=1;
    qreal m_rotationStep=1;
    qreal m_scaleFactor=1.2;
};

#endif // VIEWWIDGET_H
