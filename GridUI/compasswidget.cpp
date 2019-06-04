#include "compasswidget.h"

#include<QPaintEvent>
#include<QPainter>
#include<iostream>

namespace sliceviewer{

namespace{
QString azimuthToDirection(double az){
    while(az<0) az+=360;
    while(az>=360) az-=360;

    QString dir;
    if(az>=337.5 || az<22.5){
        dir='N';
    }
    else if(az>=22.5 && az<67.5){
        dir="NE";
    }
    else if(az>=67.5 && az<112.5){
        dir="E";
    }
    else if(az>=112.5 && az<157.5){
        dir="SE";
    }
    else if(az>=157.5 && az<202.5){
        dir="S";
    }
    else if(az>=202.5 && az<247.5){
        dir="SW";
    }
    else if(az>=247.5 && az<292.5){
        dir="W";
    }
    else if(az>=292.5 && az<337.5){
        dir="W";
    }
    return dir;
}
}

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

void CompassWidget::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
    update();
}

void CompassWidget::paintEvent(QPaintEvent* event){

    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    switch(mMode){
    case Mode::NEEDLE: drawNeedle(painter); break;
    case Mode::HORIZONTAL_DIRECTION: drawHorizontalDirection(painter); break;
    default: break;
    }
}

/*
void CompassWidget::drawNeedle(QPainter& painter){
    const int H=50;
    const int W=20;
    QPainterPath path;
    path.moveTo(0,0);
    path.lineTo(W/2,H);
    path.lineTo(0,H-W/2);
    path.lineTo(-W/2,H);
    path.lineTo(0,0);
    QTransform tf;
    tf.translate(width()/2, height()/2);
    tf.rotate(mAngle);
    tf.translate(0,-H/2);
    path=tf.map(path);
    painter.fillPath(path,mColor);
}
*/

void CompassWidget::drawNeedle(QPainter& painter){
    painter.translate(width()/2,height()/2);
    painter.rotate(mAngle);

    const int H=50;
    const int W=20;
    const int GAP=4;
    painter.setPen(QPen(Qt::black,2));
    painter.drawEllipse(-H/2,-H/2,H,H);

    QPainterPath path;
    path.moveTo(0, -H/2);
    path.lineTo(W/2,H/2);
    path.lineTo(0,H/2-W/2);
    path.lineTo(-W/2,H/2);
    path.lineTo(0,-H/2);
    painter.fillPath(path,mColor);

    painter.setFont(QFont("Times", 8, QFont::Bold));
    auto br=painter.fontMetrics().boundingRect("N");
    painter.drawText(-br.width()/2,-H/2-GAP,"N");
    br=painter.fontMetrics().boundingRect("W");
    painter.drawText(-H/2-br.width()-GAP,0+br.height()/2,"W");
    br=painter.fontMetrics().boundingRect("E");
    painter.drawText(H/2+GAP,0+br.height()/2,"E");
    br=painter.fontMetrics().boundingRect("S");
    painter.drawText(-br.width()/2,H/2+br.height(),"S");
}

void CompassWidget::drawHorizontalDirection(QPainter& painter){

    const int W=50;
    const int H=10;
    const int GAP=5;

    painter.setFont(QFont("Times", H, QFont::Bold));
    auto dir=azimuthToDirection(mAngle);
    auto br=painter.fontMetrics().boundingRect(dir);
    painter.translate((width()-W-GAP-br.width())/2,height()/2);

    painter.setPen(QPen(mColor,2));
    painter.drawLine(0,0,W-H/2,0);
    QPainterPath path;
    path.moveTo(W-H/2,-H/2);
    path.lineTo(W,0);
    path.lineTo(W-H/2,H/2);
    path.closeSubpath();
    painter.fillPath(path,mColor);
    painter.setPen(QPen(Qt::black,0));
    painter.drawText(W+GAP,br.height()/2,dir);
    std::cout<<"angle="<<mAngle<<std::endl<<std::flush;;
}

}
