#include "colorbarwidget.h"

#include<QPainter>
#include<QPaintEvent>
#include<QImage>
#include<iostream>
#include<cmath>

ColorBarWidget::ColorBarWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(50);
}


void ColorBarWidget::setColorTable( ColorTable* ct){

    if( ct==m_colorTable) return;

    m_colorTable=ct;
    m_range=ct->range();

    emit colorTableChanged( ct );

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshImage()));
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this,
             SLOT(setRange(std::pair<double,double>)));         // allways adjust colorbar to colortable
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(refreshImage()) );
    refreshImage();  // calls update
    refreshTicks();  // calls update
}

void ColorBarWidget::setTicks( const QVector<double>& ticks){

    m_ticks=ticks;

    emit ticksChanged( ticks );

    update();
}


void ColorBarWidget::setScaleAlignment(ColorBarWidget::ScaleAlignment align){

    if( align==m_scaleAlignment) return;

    m_scaleAlignment=align;

    update();
}


void ColorBarWidget::setLabel(const QString &label){

    if( label == m_label ) return;

    m_label=label;

    update();

    emit labelChanged(label);
}

void ColorBarWidget::setSteps(int n){

    if( n==m_steps) return;

    m_steps=n;

    refreshTicks();

    emit stepsChanged(m_steps);
}

void ColorBarWidget::setRange(std::pair<double, double> r){

    if( r==m_range) return;

    m_range=r;

    refreshTicks();

    emit rangeChanged(r);
}

template<typename T>
void constrain( T& value, const T& min, const T& max){

    value=(value<min)? min : ((value>max)? max : value );
}

void ColorBarWidget::paintEvent( QPaintEvent * ev){

    QVector<std::pair<int,QString>> coordAndLabel(m_ticks.size());

    QWidget::paintEvent(ev);

    QPainter painter(this);

    QSize label_size=painter.fontMetrics().size( Qt::TextSingleLine, m_label);

    double range=m_range.second - m_range.first;
    double absRange=std::fabs(m_range.first - m_range.second);
    int size_pix=height() - 2*PAD_Y;
    int dy=size_pix/m_steps;

    for( int i=0; i<m_ticks.size(); i++ ){

        double value=m_ticks[i];
        int y= PAD_Y + i*size_pix/m_steps;
        y=height()-y;                       // lowest at bottom
        if(absRange>10){
            value=std::round(value);
        }
        coordAndLabel[i]=std::pair<int, QString>( y, QString::number(value));
    }

    // determine maximum text width
    int maxWidth=0;
    for( auto const& cl : coordAndLabel ){
        int width=painter.fontMetrics().width(cl.second);
        if( width>maxWidth) maxWidth=width;
    }

    // adjust width if too small
    maxWidth+=BOX_WIDTH + label_size.height() + 10;
    if( width()<maxWidth){
        setFixedWidth(maxWidth);
    }

    // draw label
    painter.save();
    if( m_scaleAlignment==SCALE_LEFT ){
        painter.rotate( -90);
        painter.drawText( -( height() + label_size.width() )/2, label_size.height(), m_label );
    }
    else{
        painter.rotate( 90 );
        painter.drawText( (height()-label_size.width())/2, -(width()-label_size.height()) , m_label );
    }
    painter.restore();


    int boxX;
    int markX;
    int annotationX;
    int annotationWidth;
    Qt::Alignment annotationAlignment;

    if( m_scaleAlignment==SCALE_LEFT){
        boxX=width() - BOX_LINE_WIDTH - BOX_WIDTH;
        markX=boxX-MARK_SIZE/2;
        annotationX=0;
        annotationWidth=markX-5;
        annotationAlignment=Qt::AlignRight|Qt::AlignVCenter;
    }else if(m_scaleAlignment==SCALE_RIGHT){
       boxX=BOX_LINE_WIDTH;
       markX=boxX + BOX_WIDTH - BOX_LINE_WIDTH -MARK_SIZE/2;
       annotationX=markX + BOX_LINE_WIDTH + MARK_SIZE;
       annotationWidth = width() - annotationX;
       annotationAlignment=Qt::AlignLeft|Qt::AlignVCenter;
    }
    else{
        qFatal("Illegal Alignment!!!");
    }

    int boxHeight=height()-2*PAD_Y;

    QPen boxPen( Qt::black );
    boxPen.setWidth( BOX_LINE_WIDTH);
    //painter.drawPixmap( boxX, PAD_Y + BOX_LINE_WIDTH/2, BOX_WIDTH, boxHeight,
    //                    QPixmap::fromImage(m_image).scaled(BOX_WIDTH, boxHeight));

    qreal ytop=m_range.second;
    qreal ybottom=m_range.first;
    constrain(ytop, m_colorTable->range().first, m_colorTable->range().second);
    constrain(ybottom, m_colorTable->range().first, m_colorTable->range().second);

    qreal dybottom=height() - ( PAD_Y + (ybottom-m_range.first) * size_pix / absRange );
    qreal dytop=height() - ( PAD_Y + (ytop-m_range.first) * size_pix / absRange );

    qreal sytop=( ytop - m_colorTable->range().first )*m_image.height() / (m_colorTable->range().second - m_colorTable->range().first);
    qreal sybottom=( ybottom- m_colorTable->range().first )*m_image.height() / (m_colorTable->range().second - m_colorTable->range().first);

    painter.drawPixmap( boxX, dytop, BOX_WIDTH, std::fabs(dybottom-dytop),
                        QPixmap::fromImage(m_image), 0, sybottom, m_image.width(), std::fabs(sytop-sybottom));



    painter.setPen(boxPen);
    painter.drawRect( boxX, PAD_Y, BOX_WIDTH, height() - 2*PAD_Y );


    for( int i=0; i<coordAndLabel.size(); i++ ){

        const int& y=coordAndLabel[i].first;
        painter.drawLine( markX, y, markX + MARK_SIZE, y );

        painter.drawText( annotationX, y - dy/2, annotationWidth , dy, annotationAlignment, coordAndLabel[i].second); //QString::number(value) );


    }

}


void ColorBarWidget::refreshImage(){

    if( ! m_colorTable) return;

    QImage img( 1, m_colorTable->colors().size(), QImage::Format_RGB32);

    //img.setColorCount( m_colorTable->colors().size());
    //img.setColorCount( m_colorTable->colors().size());
    //img.setColorTable( m_colorTable->colors());

    for( int i=0; i<img.height(); i++){
        double v=m_colorTable->range().first + i*(m_colorTable->range().second - m_colorTable->range().first)/img.height();

        img.setPixel(0, img.height() - i -1, m_colorTable->map(v) );   // lowest at bottom
    }

    m_image=img;

    update();
}

/*
void ColorBarWidget::refreshImage(){

    if( ! m_colorTable) return;

    QImage img( 1, m_colorTable->colors().size(), QImage::Format_Indexed8);

    img.setColorCount( m_colorTable->colors().size());
    //img.setColorCount( m_colorTable->colors().size());
    img.setColorTable( m_colorTable->colors());

    for( int i=0; i<img.height(); i++){
        img.setPixel(0,i,img.height()-i-1);   // lowest at bottom
    }

    m_image=img;

    update();
}*/

void ColorBarWidget::refreshTicks(){

    //if( ! m_colorTable) return;

    if( m_steps<1 )return;  // silently ignore this for now

    QVector<double> ti(m_steps+1);


    for( int i=0; i<=m_steps; i++ ){

        //double value=m_colorTable->range().first + i*(m_colorTable->range().second - m_colorTable->range().first) / m_steps;
        double value=m_range.first + i*(m_range.second - m_range.first) / m_steps;
        ti[i]=value;
    }

    setTicks(ti);
}
