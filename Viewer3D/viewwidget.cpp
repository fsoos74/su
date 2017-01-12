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

        const qreal step=10.;
        const qreal xstep=step / m_scale.x();//  ? 1./ std::fabs(m_scale.x()) : 1;
        const qreal ystep=step / m_scale.y();//(m_scale.y()) ? 1./ std::fabs(m_scale.y()) : 1;
        const qreal zstep=step / m_scale.z();// ? 1./ std::fabs(m_scale.z()) : 1;

        switch( e->key() ){
        case Qt::Key_PageDown: m_position.setZ(m_position.z()+zstep);break;
        case Qt::Key_PageUp: m_position.setZ(m_position.z()-zstep);break;
        case Qt::Key_Up: m_position.setY(m_position.y()-ystep);break;
        case Qt::Key_Down: m_position.setY(m_position.y()+ystep);break;
        case Qt::Key_Left: m_position.setX(m_position.x()-xstep);break;
        case Qt::Key_Right: m_position.setX(m_position.x()+xstep);break;
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

    //const qreal zNear = 3.0, zFar = 7000000.0, fov = 45.0;

    qreal zNear = 3;
    qreal zFar = 7000000.0;
    //if( zNear > zFar) std::swap(zNear, zFar);
    const qreal fov=45.0;

    std::cout<<"near="<<zNear<<" far="<<zFar<<std::endl;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);



    //update();
}

/*
std::ostream& operator<<(std::ostream& os, const QVector3D& v){

    std::cout<<"[ "<<v.x()<<", "<<v.y()<<", "<<v.z()<<" ]";
    return os;
}
*/

/*
void ViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;

    std::cout<<"1: "<<m_center<<std::endl;

    //matrix.translate(m_position*m_scale);

    // rotate around x,y,z axes through volume center
    matrix.translate( m_center );

    std::cout<<"2: "<<m_center*matrix<<std::endl;

    matrix.rotate(m_rotation.x(), QVector3D( 1, 0, 0));

    //std::cout<<"3: "<<m_center*matrix<<std::endl;

    matrix.rotate(m_rotation.y(), QVector3D( 0, 1, 0));

    //std::cout<<"4: "<<m_center*matrix<<std::endl;

    matrix.rotate(m_rotation.z(), QVector3D( 0, 0, 1));

    //std::cout<<"5: "<<m_center*matrix<<std::endl;

    matrix.scale(m_scale );

    std::cout<<"7: "<<m_center*matrix<<std::endl;

    matrix.translate( -m_center);

    std::cout<<"6: "<<m_center*matrix<<std::endl;



    matrix.translate(m_position);

    std::cout<<"8: "<<m_center*matrix<<std::endl;

    m_engine->draw( m_scene, projection * matrix);

    std::cout<<"9: "<<m_center*projection*matrix<<std::endl;

}
*/

void ViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;


    matrix.translate(m_position*m_scale);


    // rotate around x,y,z axes through volume center
    matrix.translate( m_center*m_scale );
    matrix.rotate(m_rotation.x(), QVector3D( 1, 0, 0));
    matrix.rotate(m_rotation.y(), QVector3D( 0, 1, 0));
    matrix.rotate(m_rotation.z(), QVector3D( 0, 0, 1));
    matrix.translate( -m_center*m_scale);


    matrix.scale(m_scale );


    m_engine->draw( m_scene, projection * matrix);

}

/*
 * void ViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;

    //matrix.scale(m_scale ); //1., 0.1, 1. );
    //matrix.translate(m_position);//(0.0, 0.0, -5.0);
    std::cout<<"position: "<<m_position<<std::endl;
    std::cout<<"scale: "<<m_scale<<std::endl;
    std::cout<<"pos*scale: "<<m_position*m_scale<<std::endl;
    std::cout<<"center: "<<m_center<<std::endl;
    std::cout<<"center*scale: "<<m_center*m_scale<<std::endl;

    matrix.translate(m_position*m_scale);//(0.0, 0.0, -5.0);

    // rotate around x,y,z axes through volume center
    matrix.translate( m_center*m_scale );
    matrix.rotate(m_rotation.x(), QVector3D( 1, 0, 0));
    matrix.rotate(m_rotation.y(), QVector3D( 0, 1, 0));
    matrix.rotate(m_rotation.z(), QVector3D( 0, 0, 1));
    matrix.translate( -m_center*m_scale);


    matrix.scale(m_scale ); //1., 0.1, 1. );



    //matrix.translate(m_position);

    m_engine->draw( m_scene, projection * matrix);

}

*/