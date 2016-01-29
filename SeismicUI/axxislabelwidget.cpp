#include "axxislabelwidget.h"

#include<QPainter>
#include "gatherview.h"

AxxisLabelWidget::AxxisLabelWidget(GatherView* parent):QWidget(parent), m_view(parent)
{

}

void AxxisLabelWidget::setLabels(const QStringList & l){

    if( l==m_labels) return;

    m_labels=l;

    emit labelsChanged(l);
}

void AxxisLabelWidget::paintEvent(QPaintEvent*){

     QPainter painter(this);

    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    int lineDY=painter.fontMetrics().height();
    for( int i=0; i<m_labels.size(); i++){

        qreal y=height()- 8 - (m_labels.size() - i)*lineDY; // 8=1.5*TICK_MARK_SIZE
        painter.drawText( 0, y, width(), lineDY, Qt::AlignBottom|Qt::AlignRight, m_labels[i]);
    }

}


