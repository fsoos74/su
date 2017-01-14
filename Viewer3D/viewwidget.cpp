#include "viewwidget.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <math.h>
#include<iostream>

ViewWidget::ViewWidget(QWidget *parent) :
    QOpenGLWidget(parent), m_position(0.0, 0.0, -5.0), m_rotation(0.,0.,0.), m_scale(1.,1.,1.)
{
    setFocus();
}

ViewWidget::~ViewWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();

    doneCurrent();
}

void ViewWidget::setCenter(QVector3D p){

    if(p==m_center) return;

    m_center=p;

    update();
}

void ViewWidget::setPosition(QVector3D p){

    if(p==m_position) return;

    m_position=p;

    emit positionChanged(p);

    update();
}

void ViewWidget::setScale(QVector3D s){

    if(s==m_scale) return;

    m_scale=s;

    emit scaleChanged(s);

    update();
}

void ViewWidget::setRotation(QVector3D s){

    if(s==m_rotation) return;

    m_rotation=s;

    emit rotationChanged(s);

    update();
}

void ViewWidget::setDimensions(QVector3D d){

    if(d==m_dimensions) return;

    m_dimensions=d;

    emit dimensionsChanged(d);

    update();
}




std::ostream& operator<<(std::ostream& os, const QVector3D& v){

    os<<"[ "<<v.x()<<", "<<v.y()<<", "<<v.z()<<" ]";

    return os;
}


void ViewWidget::mousePressEvent(QMouseEvent *){
    setFocus();
}

void ViewWidget::keyPressEvent(QKeyEvent *e){

    if( e->modifiers()==Qt::ControlModifier ){  // zoom

        const qreal growFactor=1.25;
        const qreal shrinkFactor=0.8;

        switch( e->key() ){
        case Qt::Key_PageDown: m_scale.setZ(m_scale.z()*growFactor);break;
        case Qt::Key_PageUp: m_scale.setZ(m_scale.z()*shrinkFactor);break;
        case Qt::Key_Up: m_scale.setY(m_scale.y()*growFactor);break;
        case Qt::Key_Down: m_scale.setY(m_scale.y()*shrinkFactor);break;
        case Qt::Key_Left: m_scale.setX(m_scale.x()*shrinkFactor);break;
        case Qt::Key_Right: m_scale.setX(m_scale.x()*growFactor);break;
        }

        emit scaleChanged(m_scale);
    }
    else if( e->modifiers()==Qt::ShiftModifier){    // rotate

        const qreal step=1;

        switch( e->key() ){
        case Qt::Key_PageDown: m_rotation.setZ(m_rotation.z()+step);break;
        case Qt::Key_PageUp: m_rotation.setZ(m_rotation.z()-step);break;
        case Qt::Key_Up: m_rotation.setY(m_rotation.y()-step);break;
        case Qt::Key_Down: m_rotation.setY(m_rotation.y()+step);break;
        case Qt::Key_Left: m_rotation.setX(m_rotation.x()-step);break;
        case Qt::Key_Right: m_rotation.setX(m_rotation.x()+step);break;
        }

        emit rotationChanged(m_rotation);
    }
    else{   // move

        const qreal relativeStep=0.01;
        QVector3D step=relativeStep * m_scale * m_dimensions;

        switch( e->key() ){
        case Qt::Key_PageDown: m_position.setZ(m_position.z()-step.z());break;
        case Qt::Key_PageUp: m_position.setZ(m_position.z()+step.z());break;
        case Qt::Key_Up: m_position.setY(m_position.y()-step.y());break;
        case Qt::Key_Down: m_position.setY(m_position.y()+step.y());break;
        case Qt::Key_Left: m_position.setX(m_position.x()-step.x());break;
        case Qt::Key_Right: m_position.setX(m_position.x()+step.x());break;
        }

        emit positionChanged(m_position);
    }

    std::cout<<"position= "<<m_position<<std::endl;
    std::cout<<"rotation= "<<m_rotation<<std::endl;
    std::cout<<"scale= "<<m_scale<<std::endl;


    update();
}

void ViewWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.8, 0.8, 0.8, 1);

    //initShaders();

    glEnable(GL_DEPTH_TEST);



    //glEnable(GL_MULTISAMPLE);
    //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    // using discard in shader instead of blending to allow depth test
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);

    //glEnable(GL_CULL_FACE);       // leave this commented, want to see both faces

    m_engine = new RenderEngine;
    m_scene= new RenderScene;
}


void ViewWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // in order to avoid objects showing at the wrong depth, i.e. far objects in front of near ones
    // the znear needs to be as far as possible and the zfar should be as close as possible
    // compute nearest and farthest possible points

    qreal zNear = 1000;                 // need to compute this
    qreal zFar = 700000.0;               // should based on volume dimensions
    const qreal fov=45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}



void ViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //std::cout<<"object center: "<<m_center<<std::endl;
    //std::cout<<"scale: "<<m_scale<<std::endl;
    //std::cout<<"rotation: "<<m_rotation<<std::endl;

    // scale, rotate relative to volume center -> (0,0,0)
    // IMPORTANT: transformations have to be applied in reverse order !!!!
    QMatrix4x4 object_matrix;

    // rotate around objects x,y,z axis, first z then y then x, see above
    object_matrix.rotate(m_rotation.x(), QVector3D( 1, 0, 0));
    object_matrix.rotate(m_rotation.y(), QVector3D( 0, 1, 0));
    object_matrix.rotate(m_rotation.z(), QVector3D( 0, 0, 1));

    // scale object relative to center
    object_matrix.scale(m_scale);

    // make volume center the origin for further transformations (above, reverse order!!!)
    object_matrix.translate( -m_center );


    // this matrix is for the "camera"
    // right now the camera looks in positive z-direction (or parallel to z-axis)
    // the camera is located at m_position

    QMatrix4x4 view_matrix;
    view_matrix.translate(m_position);      // set position to center at beginning in volumeviewer

    //std::cout<<" --> "<<view_matrix * object_matrix*m_center<<std::endl;

    QMatrix4x4 matrix = projection * view_matrix * object_matrix;

    m_engine->draw( m_scene, matrix);

}

