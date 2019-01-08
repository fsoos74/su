#include "viewwidget.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <math.h>
#include<iostream>

ViewWidget::ViewWidget(QWidget *parent) :
    QOpenGLWidget(parent), m_position(0.0, 0.0, -5.0), m_rotation(0.,0.,0.), m_scale(1.,1.,1.),
    m_relativeStepSize(0.01)
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

/*
void ViewWidget::setScene(RenderScene * s){

    if( s==m_scene ) return;

    m_scene=s;

    update();
}
*/

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


void ViewWidget::setRelativeStepSize(qreal s){

    // no update required -> no check for equal
    m_relativeStepSize=s;
}


void ViewWidget::moveXPos( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.x() * m_dimensions.x();
    pos.setX( pos.x() + step );
    setPosition(pos);
}

void ViewWidget::moveXNeg( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.x() * m_dimensions.x();
    pos.setX( pos.x() - step );
    setPosition(pos);
}

void ViewWidget::moveYPos( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.y() * m_dimensions.y();
    pos.setY( pos.y() + step );
    setPosition(pos);
}

void ViewWidget::moveYNeg( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.y() * m_dimensions.y();
    pos.setY( pos.y() - step );
    setPosition(pos);
}

void ViewWidget::moveZPos( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.z() * m_dimensions.z();
    pos.setZ( pos.z() + step );
    setPosition(pos);
}

void ViewWidget::moveZNeg( ){

    QVector3D pos=position();
    qreal step = m_relativeStepSize * m_scale.z() * m_dimensions.z();
    pos.setZ( pos.z() - step );
    setPosition(pos);
}


void ViewWidget::setRotationStep(qreal s){

    // no update required -> no check for equal
    m_rotationStep=s;
}

void ViewWidget::rotateXPos( ){

    QVector3D rot=rotation();
    rot.setX( rot.x() + m_rotationStep );
    setRotation(rot);
}

void ViewWidget::rotateXNeg( ){

    QVector3D rot=rotation();
    rot.setX( rot.x() - m_rotationStep );
    setRotation(rot);
}

void ViewWidget::rotateYPos(  ){

    QVector3D rot=rotation();
    rot.setY( rot.y() + m_rotationStep );
    setRotation(rot);
}

void ViewWidget::rotateYNeg(  ){

    QVector3D rot=rotation();
    rot.setY( rot.y() - m_rotationStep );
    setRotation(rot);
}

void ViewWidget::rotateZPos(  ){

    QVector3D rot=rotation();
    rot.setZ( rot.z() + m_rotationStep );
    setRotation(rot);
}

void ViewWidget::rotateZNeg( ){

    QVector3D rot=rotation();
    rot.setZ( rot.z() - m_rotationStep );
    setRotation(rot);
}


void ViewWidget::setScaleFactor(qreal f){

    // no update required -> no check for equal
    m_scaleFactor=f;
}

void ViewWidget::scaleXPos(  ){

    QVector3D s=scale();
    s.setX( m_scale.x() * m_scaleFactor );
    if( m_tieXZScales ) s.setZ( s.z() * m_scaleFactor );
    setScale(s);
}

void ViewWidget::scaleXNeg( ){

    QVector3D s=scale();
    s.setX( s.x() / m_scaleFactor );
    if( m_tieXZScales ) s.setZ( s.z() / m_scaleFactor );
    setScale(s);
}

void ViewWidget::scaleYPos( ){

    QVector3D s=scale();
    s.setY( s.y() * m_scaleFactor );
    setScale(s);
}

void ViewWidget::scaleYNeg( ){

    QVector3D s=scale();
    s.setY( s.y() / m_scaleFactor );
    setScale(s);
}


void ViewWidget::scaleZPos( ){

    QVector3D s=scale();
    s.setZ( s.z() * m_scaleFactor );
    if( m_tieXZScales ) s.setX( s.x() * m_scaleFactor );
    setScale(s);
}

void ViewWidget::scaleZNeg( ){

    QVector3D s=scale();
    s.setZ( s.z() / m_scaleFactor );
    if( m_tieXZScales ) s.setX( s.x() / m_scaleFactor );
    setScale(s);
}


void ViewWidget::setTieXZScales(bool on){

    if( on==m_tieXZScales) return;

    m_tieXZScales=on;

    emit tieXZScalesChanged(on);
}

void ViewWidget::setBackgroundColor(QColor c){

    if( c==m_backgroundColor ) return;

    m_backgroundColor = c;

    update();
}


void ViewWidget::mousePressEvent(QMouseEvent *){
    setFocus();
}

void ViewWidget::keyPressEvent(QKeyEvent *e){

    if( e->modifiers()==Qt::ControlModifier ){  // zoom

        switch( e->key() ){
        case Qt::Key_PageDown:  scaleZPos(); break;
        case Qt::Key_PageUp:    scaleZNeg(); break;
        case Qt::Key_Up:        scaleYPos(); break;
        case Qt::Key_Down:      scaleYNeg(); break;
        case Qt::Key_Left:      scaleXNeg(); break;
        case Qt::Key_Right:     scaleXPos(); break;
        }

        //emit scaleChanged(m_scale);
    }
    else if( e->modifiers()==Qt::ShiftModifier){    // rotate

        switch( e->key() ){
        case Qt::Key_PageDown:  rotateZNeg(); break;
        case Qt::Key_PageUp:    rotateZPos(); break;
        case Qt::Key_Up:        rotateYPos(); break;
        case Qt::Key_Down:      rotateYNeg(); break;
        case Qt::Key_Left:      rotateXNeg(); break;
        case Qt::Key_Right:     rotateXPos(); break;
        }

        emit rotationChanged(m_rotation);
    }
    else{   // move

        switch( e->key() ){
        case Qt::Key_PageDown:  moveZNeg(); break;
        case Qt::Key_PageUp:    moveZPos(); break;
        case Qt::Key_Up:        moveYNeg(); break;
        case Qt::Key_Down:      moveYPos(); break;
        case Qt::Key_Left:      moveXNeg(); break;
        case Qt::Key_Right:     moveXPos(); break;
        }

        emit positionChanged(m_position);
    }

}

void ViewWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
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

    // set background color
    glClearColor( m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1);

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
