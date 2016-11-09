#include "compasswidget.h"

#include<QPainter>
#include<QPainterPath>

CompassWidget::CompassWidget(QWidget *parent) : QWidget(parent)
{
}


void CompassWidget::setAzimuth(qreal a){

    if( a==m_azimuth) return;

    m_azimuth=a;

    update();

    emit azimuthChanged(a);
}


void CompassWidget::setS1(int s){

    if( s==m_s1) return;

    m_s1=s;

    update();

    emit s1Changed(s);

}


void CompassWidget::paintEvent(QPaintEvent *){

    QPainter painter(this);

    qreal S2=m_s1/3;

    painter.translate(width()/2, height()/2);
    painter.rotate(m_azimuth);

    QPainterPath path;

    path.moveTo( 0, -m_s1/2);
    path.lineTo( -S2/2, m_s1/2);
    path.lineTo( 0, m_s1/2-S2/2);
    path.lineTo( S2/2, m_s1/2);
    path.lineTo( 0, -m_s1/2);


    painter.fillPath(path, Qt::red);
    painter.setPen(QPen(Qt::black,0));
    painter.drawPath(path);
}
