#include "histogramcolorbarwidget.h"

#include<QPainter>

HistogramColorBarWidget::HistogramColorBarWidget(QWidget *parent) : QWidget(parent)
{

}


void HistogramColorBarWidget::setData( float* data_beg, size_t data_count, float null_value ){

    m_data_beg=data_beg;
    m_data_count=data_count;
    m_null_value=null_value;
    updateHistogram();
}

void HistogramColorBarWidget::setHistogram(Histogram h){
    m_histogram=h;
    update();
}

void HistogramColorBarWidget::setColorTable( ColorTable* ct){

    if( ct==m_colorTable ) return;
    if( m_colorTable ) disconnect(m_colorTable, 0, 0, 0);
    m_colorTable=ct;
    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(update()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateHistogram()) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(update()) );
}

void HistogramColorBarWidget::setOrientation(Qt::Orientation o){
    if(o==m_orientation) return;
    m_orientation=o;
    update();
}

void HistogramColorBarWidget::setScalePrecision(int p){
    if(p==m_scalePrecision) return;
    m_scalePrecision=p;
    update();
}

void HistogramColorBarWidget::paintEvent(QPaintEvent*){

    QPainter painter(this);

    // fill with non-uniform background such that all colors from colortable will show
    painter.fillRect( rect(), QBrush( Qt::darkGray, Qt::Dense4Pattern ) );

    // find max
    Histogram::const_iterator it = std::max_element( m_histogram.cbegin(), m_histogram.cend() );
    if( it == m_histogram.cend() || *it==0) return;
    auto maxCount=*it;

    if(m_orientation==Qt::Vertical){
        // draw bars
        painter.save();
        painter.translate( width(), 0 );
        painter.scale( -qreal(width())/maxCount, qreal(height())/m_histogram.binCount());
        for( size_t i = 0; i<m_histogram.binCount(); i++){
            auto interval=m_histogram.binInterval(i);
            auto midValue=(interval.first+interval.second)/2;
            QColor color = (m_colorTable) ? m_colorTable->map( midValue ) : Qt::red;
            painter.setPen( color );
            painter.drawLine( 0, i, m_histogram.binValue(i), i );
        }
        painter.restore();

        //draw range
        painter.drawText(0,0,width(),height(),Qt::AlignTop|Qt::AlignLeft, QString::number(m_colorTable->range().first,'g',m_scalePrecision));
        painter.drawText(0,0,width(),height(),Qt::AlignBottom|Qt::AlignLeft, QString::number(m_colorTable->range().second,'g',m_scalePrecision));
    }
    else{  // horizontal
        // draw bars
        painter.save();
        painter.translate(0,height());
        painter.scale( qreal(width())/m_histogram.binCount(), -qreal(height())/maxCount);
        for( size_t i = 0; i<m_histogram.binCount(); i++){
            auto interval=m_histogram.binInterval(i);
            auto midValue=(interval.first+interval.second)/2;
            QColor color = (m_colorTable) ? m_colorTable->map( midValue ) : Qt::red;
            painter.setPen( color );
            painter.drawLine( i, 0, i, m_histogram.binValue(i));
        }
        painter.restore();

        //draw range
        /*
        painter.save();
        painter.rotate(-90);
        painter.translate(-height(),0);
        painter.drawText(0,0,height(),width(),Qt::AlignTop|Qt::AlignRight, QString::number(m_colorTable->range().first,'g',m_scalePrecision));
        painter.drawText(0,0,height(),width(),Qt::AlignBottom|Qt::AlignRight, QString::number(m_colorTable->range().second,'g',m_scalePrecision));
        painter.restore();
        */
        painter.drawText(0,0,width(),height(),Qt::AlignTop|Qt::AlignLeft, QString::number(m_colorTable->range().first,'g',m_scalePrecision));
        painter.drawText(0,0,width(),height(),Qt::AlignTop|Qt::AlignRight, QString::number(m_colorTable->range().second,'g',m_scalePrecision));
    }

}

void HistogramColorBarWidget::updateHistogram(){
    if(!m_data_beg || m_data_count<1 ) return;
    auto h=createHistogram( m_data_beg, m_data_beg + m_data_count, m_null_value,
                                 (float)m_colorTable->range().first, (float)m_colorTable->range().second, 100 );
    setHistogram(h);
}
