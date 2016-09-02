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

    emit colorTableChanged( ct );

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshImage()));
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshTicks()));

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

void ColorBarWidget::paintEvent( QPaintEvent * ev){
/*
    const int N_STEPS=10;
    const int BOX_WIDTH=30;
    const int BOX_LINE_WIDTH=2;
    const int MARK_SIZE=10;
    const int PAD_Y=10;
*/
    QVector<std::pair<int,QString>> coordAndLabel(m_ticks.size());

    QWidget::paintEvent(ev);

    QPainter painter(this);

    QSize label_size=painter.fontMetrics().size( Qt::TextSingleLine, m_label);

    double range=m_colorTable->range().second - m_colorTable->range().first;
    double absRange=std::fabs(m_colorTable->range().first - m_colorTable->range().second);
    int size_pix=height() - 2*PAD_Y;
    int dy=size_pix/N_STEPS;

    for( int i=0; i<m_ticks.size(); i++ ){

        double value=m_ticks[i];
        int y= PAD_Y + i*size_pix/N_STEPS;
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
    painter.drawPixmap( boxX, PAD_Y + BOX_LINE_WIDTH/2, BOX_WIDTH, boxHeight,
                        QPixmap::fromImage(m_image).scaled(BOX_WIDTH, boxHeight));

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

    QImage img( 1, m_colorTable->colors().size(), QImage::Format_Indexed8);

    img.setColorCount( m_colorTable->colors().size());
    img.setColorTable( m_colorTable->colors());

    for( int i=0; i<img.height(); i++){
        img.setPixel(0,i,img.height()-i-1);   // lowest at bottom
    }

    m_image=img;

    update();
}

void ColorBarWidget::refreshTicks(){

    if( ! m_colorTable) return;

    QVector<double> ti(N_STEPS+1);


    for( int i=0; i<=N_STEPS; i++ ){

        double value=m_colorTable->range().first + i*(m_colorTable->range().second - m_colorTable->range().first) / N_STEPS;
        ti[i]=value;
    }

    setTicks(ti);
}
