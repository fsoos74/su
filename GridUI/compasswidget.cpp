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

void CompassWidget::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
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


}
