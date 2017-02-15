#include "histogramwidget.h"

#include<QPainter>
#include<QMouseEvent>

#include <algorithm>
#include <cmath>

#include <iostream>


// all exchange done via colortable range

HistogramWidget::HistogramWidget(QWidget* parent) : QWidget(parent)
{

}


void HistogramWidget::setHistogram(const Histogram& h){

    m_histogram=h;
    update();
}

void HistogramWidget::setColorTable( ColorTable* ct){

    if( ct==m_colorTable ) return;

    if( m_colorTable ) disconnect(m_colorTable, 0, 0, 0);

    m_colorTable=ct;

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(update()) );

    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)),
             this, SLOT(setRange(std::pair<double,double>)) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(setPower(double)) );

    connect( this, SIGNAL(rangeChanged(std::pair<double,double>)),
             m_colorTable, SLOT(setRange(std::pair<double, double>) ) );
    connect( this, SIGNAL(powerChanged(double)),
             m_colorTable, SLOT(setPower(double) ) );

    setRange( m_colorTable->range() );
    setPower( m_colorTable->power() );
}

void HistogramWidget::setRange(std::pair<double, double> r){

    if( r==m_range) return;

    m_range=r;

    update();

    if( m_instantUpdates){
        emit rangeChanged(r);
    }
}

void HistogramWidget::setPower(double p){

    if( p==m_power ) return;

    m_power=p;

    update();

    emit powerChanged(p);
}

void HistogramWidget::setInstantUpdates(bool on){

    if( m_instantUpdates==on ) return;

    m_instantUpdates=on;

    emit instantUpdatesChanged(on);
}

void HistogramWidget::paintEvent(QPaintEvent*){

    QPainter painter(this);

    // fill with non-uniform background such that all colors from colortable will show
    painter.fillRect( rect(), QBrush( Qt::darkGray, Qt::Dense4Pattern ) );

    // find max
    Histogram::const_iterator it = std::max_element( m_histogram.cbegin(), m_histogram.cend() );
    if( it == m_histogram.cend() || *it==0) return;
    auto maxCount=*it;

    // draw bars
    painter.save();

    painter.translate( 0, height() );
    painter.scale( qreal(width())/m_histogram.binCount(), -qreal(height())/maxCount);

    for( size_t i = 0; i<m_histogram.binCount(); i++){
        auto interval=m_histogram.binInterval(i);
        auto midValue=(interval.first+interval.second)/2;
        QColor color = (m_colorTable) ? m_colorTable->map( midValue ) : Qt::red;
        painter.setPen( color );
        painter.drawLine( i, 0, i, m_histogram.binValue(i) );
    }

    painter.restore();


    // draw min/max indicators
    QPen indicatorPen(Qt::black);
    indicatorPen.setCosmetic(true);

    int imin=valueToX( std::max(m_range.first, m_histogram.firstBin() ) );
    painter.drawLine( imin, 0, imin, height() );
    int imax=valueToX( std::min( m_range.second, m_histogram.firstBin() + m_histogram.binWidth() * m_histogram.binCount() ) );
    painter.drawLine( imax, 0, imax, height() );

    // draw curve
    QPainterPath path;
    for( int i = imin; i<=imax; i++){

        double value=xToValue(i);
        double tvalue = height()  * ( 1. - std::pow( (value - m_range.first)/(m_range.second-m_range.first), m_power) );

        if( i!=imin ){
            path.lineTo( i, tvalue );
        }
        else{
            path.moveTo( i, height() );     // don't compute first value because std::pow -> nan for almost zero numbers
        }

    }
    painter.strokePath(path, QPen(Qt::black,0) );

}


void HistogramWidget::mousePressEvent(QMouseEvent * event){

    double value = xToValue( event->x() );

    // set mode based on nearest indicator
    m_mode = ( std::fabs(m_range.first-value) < std::fabs( m_range.second-value) ) ?
                   MouseMode::EditMinimum : MouseMode::EditMaximum;

    handleMouseEvent(event);
}

void HistogramWidget::mouseMoveEvent(QMouseEvent * event){

    handleMouseEvent(event);
}

void HistogramWidget::mouseReleaseEvent(QMouseEvent *event){
    if( !m_instantUpdates ){
        emit rangeChanged(m_range);     // need to refresh external viewers
        update();
    }
    m_mode = MouseMode::None;
}

void HistogramWidget::leaveEvent(QEvent *){
    m_mode = MouseMode::None;
}

void HistogramWidget::handleMouseEvent(QMouseEvent * event){

    // ignore if moved out of window
    if( !rect().contains( event->x(), event->y() ) ){
        return;
    }

    double value = xToValue( event->x() );

    if( m_mode==MouseMode::EditMinimum ){  // set min
        setRange( std::make_pair( value, m_range.second) );
    }
    else if( m_mode==MouseMode::EditMaximum){ // maximum
        setRange( std::make_pair( m_range.first, value ) );
    }
}


double HistogramWidget::xToValue(int x) const{

    return m_histogram.firstBin() + double(x)/width()*m_histogram.binWidth()*m_histogram.binCount();
}

int HistogramWidget::valueToX(double value) const{

    return (value-m_histogram.firstBin())/m_histogram.binWidth()/m_histogram.binCount() * width();
}
