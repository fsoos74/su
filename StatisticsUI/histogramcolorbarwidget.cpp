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

void HistogramColorBarWidget::setColorTable( ColorTable* ct){

    if( ct==m_colorTable ) return;

    if( m_colorTable ) disconnect(m_colorTable, 0, 0, 0);

    m_colorTable=ct;

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(update()) );

    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updateHistogram()) );

    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(update()) );
}


void HistogramColorBarWidget::paintEvent(QPaintEvent*){

    QPainter painter(this);

    // fill with non-uniform background such that all colors from colortable will show
    painter.fillRect( rect(), QBrush( Qt::darkGray, Qt::Dense4Pattern ) );

    // find max
    Histogram::const_iterator it = std::max_element( m_histogram.cbegin(), m_histogram.cend() );
    if( it == m_histogram.cend() || *it==0) return;
    auto maxCount=*it;

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

}

void HistogramColorBarWidget::updateHistogram(){
    m_histogram=createHistogram( m_data_beg, m_data_beg + m_data_count, m_null_value,
                                 (float)m_colorTable->range().first, (float)m_colorTable->range().second, 100 );
    update();
}
