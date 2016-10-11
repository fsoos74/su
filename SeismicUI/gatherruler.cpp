#include "gatherruler.h"

#include "gatherview.h"
#include "gatherlabel.h"
#include "headerui.h"
#include<QPainter>
#include<QScrollBar>
#include<QMouseEvent>

#include<iostream>

GatherRuler::GatherRuler(GatherView *parent, Orientation orient) : QWidget(parent), m_view(parent), m_orientation(orient)
{

}


void GatherRuler::setAxxisLabels(const QStringList & l){

    if( l==m_axxisLabels) return;

    m_axxisLabels=l;

    emit axxisLabelsChanged(m_axxisLabels);

    update();
}

void GatherRuler::setCurrentPos(qreal x){

    if( x==m_currentPos) return;

    m_currentPos=x;

    update();
}

QVector< GatherRuler::Tick > GatherRuler::computeTicks( int pixFrom, int pixTo )const{

    QVector< Tick > ticks;

    if( m_orientation==VERTICAL_RULER){

        // we are using milliseconds so that we can use integer arithmetic
        qreal firstMill=1000*m_view->ft();
        qreal pixelPerMill=0.001*m_view->pixelPerSecond();

        int millsFrom=firstMill + pixFrom / pixelPerMill;
        int millsTo=  firstMill + pixTo / pixelPerMill;

        int incr=tickIncrement( pixTo - pixFrom + 1, millsTo - millsFrom, TICK_LABEL_DY);


        if( incr > 0 ){

            for( int mills=incr*(millsFrom/incr+1); mills<millsTo; mills+=incr){

                int pix=(mills - firstMill)*pixelPerMill;
                ticks.push_back( Tick( pix, QString::number(mills) ));
            }
        }

    }
    else{

        qreal pixelPerTrace=m_view->pixelPerTrace();
        qreal traceFrom=pixFrom/pixelPerTrace;
        qreal traceTo=pixTo/pixelPerTrace;

        // we are using integer trace numbers!!
        int incr=tickIncrement( pixTo - pixFrom + 1, traceTo - traceFrom + 1, TICK_LABEL_DX);

        if( incr > 0 ){

            for( int trace=incr*(std::floor(traceFrom/incr)+1); trace<traceTo; trace+=incr){

                int pix=(trace+0.5)*pixelPerTrace;      // tick in middle of trace
                ticks.push_back( Tick( pix, m_view->traceAnnotation(trace) ));
            }
        }
    }


    return ticks;

}




void GatherRuler::paintEvent(QPaintEvent*){

    QPainter painter(this);

    painter.setPen(Qt::black);

    if( m_orientation==VERTICAL_RULER){
        drawVertical( painter );
    }
    else{
        drawHorizontal( painter );
    }

}


void GatherRuler::drawVerticalIndicator(QPainter& painter, QPoint p){

    const int INDICATOR_SIZE=4;

    if( p.y()<-INDICATOR_SIZE || p.y()>height()+INDICATOR_SIZE ) return;

    QPolygon poly;
    poly<<QPoint( 0, 0 )<<QPoint( -INDICATOR_SIZE, - INDICATOR_SIZE)<<QPoint( -INDICATOR_SIZE, INDICATOR_SIZE);
    poly.translate( p );
    painter.drawPolygon(poly);
}

void GatherRuler::drawVertical( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    QScrollBar* sb=m_view->verticalScrollBar();
    if( !sb ){
        qDebug("sb == 0");
        return;
    }

    // drawaxxis label
    if( !m_axxisLabels.empty() ){
        painter.save();
        painter.rotate( -90);
        QSize text_size=painter.fontMetrics().size( Qt::TextSingleLine, m_axxisLabels[0]);
        painter.drawText( -( height() + text_size.width() )/2, text_size.height(), m_axxisLabels[0] );         // center vertically
        painter.restore();
    }

    QVector<Tick> ticks=computeTicks( sb->value(), sb->value() + height());// cannot use pageStep because scrollbars are adjusted after all event processing


    int x1=width();
    int x2=x1-1.5*TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int y=tick.coord() - sb->value();

        painter.drawLine( x1, y , x2, y );

        if( !tick.annotations().empty()){
            painter.drawText( 0, y - TICK_LABEL_DY/2, x2, TICK_LABEL_DY, Qt::AlignRight|Qt::AlignVCenter, tick.annotations()[0] );
        }
    }


    qreal y=(m_currentPos - m_view->ft())*m_view->pixelPerSecond() - sb->value();
    drawVerticalIndicator( painter, QPoint( width()-1, y) );
}

void GatherRuler::drawHorizontalIndicator(QPainter& painter, QPoint p){

    const int INDICATOR_SIZE=4;

    if( p.x()<-INDICATOR_SIZE || p.x()>width()+INDICATOR_SIZE) return;

    QPolygon poly;
    poly<<QPoint( 0, 0 )<<QPoint( -INDICATOR_SIZE, - INDICATOR_SIZE)<<QPoint( INDICATOR_SIZE, -INDICATOR_SIZE);
    poly.translate( p );
    painter.drawPolygon(poly);
}

void GatherRuler::drawHorizontal( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    QScrollBar* sb=m_view->horizontalScrollBar();
    if( !sb ){
        qDebug("sb == 0");
        return;
    }

    int lineDY=painter.fontMetrics().height();


    QVector<Tick> ticks=computeTicks( sb->value(), sb->value() + width());// cannot use pageStep because scrollbars are adjusted after all event processing

    int y1=height();
    int y2=y1-1.5*TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int x=tick.coord() - sb->value();
        painter.drawLine( x, y1, x, y2 );
        //painter.drawText( x - TICK_LABEL_DX/2, 0, TICK_LABEL_DX, y2, Qt::AlignBottom|Qt::AlignHCenter, tick.string() );
        const QStringList& annos=tick.annotations();
        for( int i=0; i<annos.size(); i++){

            qreal y3=y2- (annos.size() - i-1)*lineDY;
            painter.drawText( x - TICK_LABEL_DX/2, 0, TICK_LABEL_DX, y3, Qt::AlignBottom|Qt::AlignHCenter, annos[i]);
        }

    }


    qreal x=(m_currentPos + 0.5)*m_view->pixelPerTrace();
    drawHorizontalIndicator( painter, QPoint( x, height()-1) );
}


int GatherRuler::tickIncrement(int size_pix, int size_data, int min_distance)const{
//std::cout<<"computeTickIncrement: size_data="<<size_data<<" size_pix="<<size_pix<<std::endl;
    double pix_per_unit=double(size_pix)/size_data;
    int incr= std::pow(10,std::floor(std::log10(min_distance/pix_per_unit)));

    if( pix_per_unit>=min_distance){
        incr=1;
    }
    else if( 2*incr*pix_per_unit>=min_distance){
        incr*=2;
    }
    else  if( 5*incr*pix_per_unit>=min_distance){
        incr*=5;
    }
    else  if( 10*incr*pix_per_unit>=min_distance){
        incr*=10;
    }
    else{

        qWarning("No tick increment found!");
        incr=-1;
    }

    return incr;
}

