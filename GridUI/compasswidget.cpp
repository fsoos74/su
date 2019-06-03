#include "compasswidget.h"

#include<QPaintEvent>
#include<QPainter>

namespace sliceviewer{

CompassWidget::CompassWidget(QWidget *parent) : QWidget(parent)
{

}

void CompassWidget::setMode(Mode mode){
    if(mode==mMode) return;
    mMode=mode;
    update();
}

void CompassWidget::setAngle(float angle){
    if(angle==mAngle) return;
    mAngle=angle;
    update();
}

void CompassWidget::paintEvent(QPaintEvent* event){

    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    switch(mMode){
    case Mode::NEEDLE: drawNeedle(painter); break;
    default: break;
    }
}

void CompassWidget::drawNeedle(QPainter& painter){
    const int H=50;
    const int W=20;
    QPainterPath path;
    path.moveTo(0,0);
    path.lineTo(W/2,H);
    path.lineTo(0,H-W);
    path.lineTo(-W/2,H);
    path.lineTo(0,0);
    QTransform tf;
    tf.translate(width()/2, height()/2);
    tf.rotate(mAngle);
    tf.translate(0,-H/2);
    path=tf.map(path);
    painter.fillPath(path,Qt::blue);
}

}
