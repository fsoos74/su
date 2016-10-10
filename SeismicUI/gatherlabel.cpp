#include "gatherlabel.h"
#include "gatherruler.h"
#include "gatherview.h"
#include<colortable.h>
#include<grid2d.h>

#include<QPainter>
#include<QPaintEvent>
#include<iostream>
#include<QScrollBar>
#include<QApplication>


GatherLabel::GatherLabel( GatherView* parent ) : QLabel(parent), m_view(parent),
    m_colorTable(new ColorTable(this)),
    m_volumeColorTable( new ColorTable(this))
{
    m_colorTable->setRange(-1, 1);
    QVector<QRgb> baseColors=ColorTable::grayscale();
    m_colorTable->setColors(baseColors);
    connect( m_colorTable, SIGNAL(colorsChanged()), this,SLOT(updateBuffers()));
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this,SLOT(updateBuffers()));

    m_volumeColorTable->setRange(-1, 1);
    QVector<QRgb> volumeBaseColors=ColorTable::defaultColors();
    m_volumeColorTable->setColors(volumeBaseColors);
    connect( m_volumeColorTable, SIGNAL(colorsChanged()), this,SLOT(updateBuffers()));
    connect( m_volumeColorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this,SLOT(updateBuffers()));
}


 void GatherLabel::setDisplayDensity(bool on){

     if( on==m_density) return;

     m_density=on;

     emit displayDensityChanged(on);

     updateBuffers();
 }

 void GatherLabel::setDisplayWiggles(bool on){

     if( on==m_wiggles ) return;

     m_wiggles=on;

     emit displayWigglesChanged(on);

     updateBuffers();
 }

 void GatherLabel::setDisplayVariableArea(bool on){

     if( on==m_variableArea) return;

     m_variableArea=on;

     emit displayVariableAreaChanged(on);

     updateBuffers();
 }

 void GatherLabel::setDisplayLines(bool on){

     if( on==m_displayLines) return;

     m_displayLines=on;

     update();

     emit displayLinesChanged(on);
 }

void GatherLabel::setHighlightTrace( bool on){

    if( on==m_highlightTrace) return;

    m_highlightTrace=on;

    //updatePixmap();
    updateBuffers();
}

void GatherLabel::setHighlightedTrace( size_t t){

    if( t == m_highlightedTrace) return;

    m_highlightedTrace=t;

   updateBuffers();
}

void GatherLabel::setViewerCurrentTrace( int t){

    if( t == m_viewerCurrentTrace) return;

    m_viewerCurrentTrace=t;

    update();
}

void GatherLabel::setHorizonColor(QColor color){

    if( color==m_horizonColor) return;

    m_horizonColor=color;

    update();

    emit horizonColorChanged(color);
}

void GatherLabel::setHighlightedPointSize(int s){

    if( s==m_highlightedPointSize) return;

    m_highlightedPointSize=s;

    update();
}

void GatherLabel::setHighlightedPointColor(QColor c){

    if( c==m_highlightedPointColor ) return;

    m_highlightedPointColor=c;

    update();
}

void GatherLabel::setTraceColor(QColor rgb){

    if(rgb==m_traceColor) return;

    m_traceColor=rgb;
    updateBuffers();
}

void GatherLabel::setTraceOpacity(int o){

    if(o==m_traceOpacity) return;

    m_traceOpacity=o;
    updateBuffers();
}

void GatherLabel::setDensityOpacity(int o){

    if(o==m_densityOpacity) return;

    m_densityOpacity=o;
    updateBuffers();
}

void GatherLabel::setVolumeOpacity(int o){

    if(o==m_volumeOpacity) return;

    m_volumeOpacity=o;
    updateBuffers();
}


void GatherLabel::onViewGatherChanged(std::shared_ptr<seismic::Gather>){

    buildTraceLookup();

    updateTraceScaleFactors();  // this will call updatePixmap and update

}


void GatherLabel::updateTraceScaleFactors(){

    Q_ASSERT( m_view );

    if( !m_view->gather()) return;

    traceScaleFactors=m_view->gatherScaler()->computeScalingFactors(m_view->gather());

    updateBuffers();
}


void GatherLabel::paintEvent(QPaintEvent *event){


    Q_ASSERT( m_view );

    QPainter painter(this);

    QRect rect=event->rect();

    QPoint topLeft=QPoint( m_view->horizontalScrollBar()->value(), m_view->verticalScrollBar()->value());

    if( topLeft!=m_pixmapTopLeft || m_dirty){
        updatePixmap();
    }

    if( m_dirty ){
        painter.drawPixmap(m_pixmapTopLeft, m_pixmap);
    }
    else{
        painter.drawPixmap(rect.x(), rect.y(), m_pixmap,
                       rect.x()-m_pixmapTopLeft.x(), rect.y()-m_pixmapTopLeft.y(), rect.width(), rect.height());
    }

    if( m_displayLines){
        drawHorizontalLines( painter, rect );
    }

   drawHorizons( painter );

   drawHighlightedPoints( painter, m_view->highlightedPoints() );

   drawIntersectionTraces( painter, m_view->intersectionTraces() );


   SelectionPoint sp=m_view->viewerCurrentPoint();
   drawViewerCurrentPosition( painter, sp);

   m_dirty=false;
 }


void GatherLabel::drawHorizons( QPainter& painter ){

    //static const QVector<QColor> HorizonColors{Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::magenta, Qt::cyan};

    painter.save();

   for( int i=0; i<m_view->horizonList().size(); i++){

       QString name=m_view->horizonList()[i];
       QColor color=m_view->horizonColor(name); //HorizonColors[ i % HorizonColors.size() ];
       std::shared_ptr<Grid2D<float>> horizon=m_view->horizon(name);
       if( !horizon ) continue;

        QPen thePen( color, 2);

        drawHorizon( painter, horizon, thePen  );
  }

   painter.restore();

}


void GatherLabel::drawHorizon(QPainter& painter, std::shared_ptr<Grid2D<float>> g, QPen thePen){

     if(!m_view->gather()) return;
    const seismic::Gather& gather=*(m_view->gather() );

    if( !g ) return;
    const Grid2D<float>& horizon=*g;

    qreal pixelPerTrace=m_view->pixelPerTrace();
    qreal pixelPerSecond=m_view->pixelPerSecond();
    qreal ft=m_view->ft();

   QPainterPath path;
   bool active=false;
   for( size_t i=0; i<gather.size(); i++){  // XXX

       const seismic::Trace& trace=gather[i];
       const seismic::Header& header=trace.header();
       int iline=header.at("iline").intValue();
       int xline=header.at("xline").intValue();
       //qreal t=horizon.bounds().isInside(iline,xline) ?           // need to check this here because it is not checked in grid
       //            horizon( iline, xline ) : horizon.NULL_VALUE;  // maybe add a getValue function to grid that includes this check

       qreal t=horizon.valueAt(iline, xline);

       if( t==horizon.NULL_VALUE){

           active=false;
           continue;
       }

       t*=0.001; // test hrz was mills change!!!
       qreal x= i * pixelPerTrace;
       qreal y= ( t - ft )*pixelPerSecond;


       if( active ){
            path.lineTo(x,y);
       }else{
           path.moveTo(x,y);
           active=true;
       }

   }

   painter.strokePath(path, thePen);
}

// currently virtual cdps numbers are used. PROBLEMS WHEN INLINE NUMBERS >999999!!!!! XXX
int ilxlToCDP( int iline, int xline){
    const int IL_FACTOR=1000000;
    return IL_FACTOR*iline + xline;
}

// create a lookup table to map cdps to traces
void GatherLabel::buildTraceLookup(){

    traceLookup.clear();
    if(!m_view->gather()) return;

    const seismic::Gather& gather=*(m_view->gather() );

    for( size_t i=0; i<gather.size(); i++){  // XXX

        const seismic::Trace& trace=gather[i];
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();
        int cdp=ilxlToCDP(iline, xline);
        traceLookup.insert(cdp,i);
    }
}

// returns trace number (in gather) of first trace with iline, xline
// return value -1 if no such trace
int GatherLabel::lookupTrace(int iline, int xline){

    int cdp=ilxlToCDP(iline, xline);

    if( traceLookup.contains(cdp)){

        return traceLookup.value(cdp);
    }

    return -1;
}


void GatherLabel::drawHighlightedPoints( QPainter& painter,
                                         const QVector<SelectionPoint>& points){

    if(!m_view->gather()) return;


    painter.save();

    painter.setPen( m_highlightedPointColor );
    painter.setBrush(QBrush(m_highlightedPointColor));
    qreal pixelPerTrace=m_view->pixelPerTrace();
    qreal pixelPerSecond=m_view->pixelPerSecond();
    qreal ft=m_view->ft();


    for( SelectionPoint p : points){

        int trc=lookupTrace( p.iline, p.xline );
        if( trc < 0 ) continue;           // no trace in gather with that cdp

        qreal x= trc * pixelPerTrace;
        qreal y= ( p.time - ft )*pixelPerSecond;

        painter.drawEllipse( QPointF(x,y) ,m_highlightedPointSize, m_highlightedPointSize);
    }

    painter.restore();

}


void GatherLabel::drawViewerCurrentPosition( QPainter& painter, SelectionPoint sp){

    const int CURSOR_SIZE=5;
    const int CURSOR_PEN_WIDTH=3;

    if( sp==SelectionPoint::NONE) return;   // maybe skip this because iline and xline will give incalid trace number which is checked anyway

    int trc=m_view->lookupTrace(  sp.iline, sp.xline);
    if( trc == -1 || sp.time==SelectionPoint::NO_TIME) return;

    painter.save();

    painter.setPen( QPen(Qt::red, CURSOR_PEN_WIDTH ) );  // make this selectable

    qreal x= ( trc + 0.5 ) * m_view->pixelPerTrace(); // center of trace bin
    qreal pixelPerSecond=m_view->pixelPerSecond();
    qreal ft=m_view->ft();
    qreal y= ( sp.time - ft )*pixelPerSecond;

    painter.drawLine( x - CURSOR_SIZE, y, x+CURSOR_SIZE, y );
    painter.drawLine( x, y-CURSOR_SIZE, x, y+CURSOR_SIZE);

    painter.restore();

}


void GatherLabel::drawIntersectionTraces( QPainter& painter, const QVector<int>& trc){

    painter.save();

    painter.setPen( QPen(Qt::blue, 0) );  // make this selectable
    //painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Xor );
    qreal pixelPerTrace=m_view->pixelPerTrace();

    for( auto t : trc ){

        qreal x= t * pixelPerTrace +0.5*pixelPerTrace ; // center
        painter.drawLine( x, 0, x, height());
    }

    painter.restore();

}

void GatherLabel::drawHorizontalLines(QPainter& painter, const QRect& rect){


    GatherRuler* ruler=m_view->leftRuler();
    QVector< GatherRuler::Tick > ticks= ruler->computeTicks( rect.top(), rect.bottom() );

    painter.save();

    QPen thePen( Qt::black, 0);

    painter.setPen( thePen);

    for( const GatherRuler::Tick& tick:ticks){
        painter.drawLine( rect.left(), tick.coord(), rect.right(), tick.coord() );
    }

    painter.restore();

}

void GatherLabel::updatePixmap(){

    const int MAX_PIXMAP_WIDTH=32767;
    const int MAX_PIXMAP_HEIGHT=32767;

     Q_ASSERT( m_view );

    QPoint topLeft=QPoint( m_view->horizontalScrollBar()->value(), m_view->verticalScrollBar()->value());
     m_pixmapTopLeft=topLeft;

    if( m_pixmap.isNull() || m_pixmap.size()!=m_view->viewport()->size()){
        m_pixmap=QPixmap( m_view->viewport()->size());
    }

     m_pixmap.fill(Qt::white);

    if( !m_view->gather() || m_view->gather()->size()<1) return;


    QPainter painter(&m_pixmap);

    painter.translate( -m_pixmapTopLeft.x(), -m_pixmapTopLeft.y());

    // draw density if required
    if( m_density){

        painter.setOpacity(qreal(densityOpacity())/100);

        if( width()<MAX_PIXMAP_WIDTH && height()<MAX_PIXMAP_HEIGHT){     // could be null if maximum size is exceeded
            QPixmap scaledImagePixmap=m_densityPlot.scaled( size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap( 0, 0, scaledImagePixmap );
         }
        else{
            painter.fillRect( QRect(0,0,width(),height()), QBrush( Qt::gray, Qt::SolidPattern) );
        }
        painter.setOpacity(1);

        // draw highlighted trace, XXX need to move this to end !!!
        if( m_highlightTrace){
            QPen thePen( Qt::red, 0);
            painter.setPen(thePen);
            qreal x=(m_highlightedTrace+0.5)*m_view->pixelPerTrace();
            painter.drawLine( x, 0, x, m_pixmap.height());
        }
    }

    // draw volume overlay if required
    if(m_view->volume() &&  width()<MAX_PIXMAP_WIDTH && height()<MAX_PIXMAP_HEIGHT){
        const Grid3D<float>& volume=*(m_view->volume());
        const seismic::Trace& trace=m_view->gather()->front();

        double ft=volume.bounds().ft();
        double lt=volume.bounds().lt();

        double timeRatio=(lt-ft)/(trace.lt()-trace.ft());
        int h=timeRatio*height(); // XXX
        double deltaTime=ft - trace.ft();
        int y=deltaTime*m_view->pixelPerSecond();

        QPixmap scaledVolumePixmap=m_volumePixmap.scaled( QSize( width(), h), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.setOpacity(qreal(volumeOpacity())/100);
        painter.drawPixmap( 0, y, scaledVolumePixmap );             // XXXX
        painter.setOpacity(1);
    }

    const seismic::Gather& gather=*m_view->gather();

    size_t x1=static_cast<size_t>(m_view->horizontalScrollBar()->value());
    size_t x2=x1 + static_cast<size_t>(m_view->viewport()->width());
    size_t firstTrace=x1/m_view->pixelPerTrace();
    if( firstTrace>0) firstTrace--;
    size_t lastTrace=x2/m_view->pixelPerTrace() + 1;
    if(lastTrace>=gather.size()) lastTrace=gather.size()-1;

    painter.setRenderHint(QPainter::Antialiasing);

    qreal pixelPerTrace=m_view->pixelPerTrace();
    qreal pixelPerSecond=m_view->pixelPerSecond();
    qreal ft=m_view->ft();

    QPen thePen(traceColor(),0);
    QBrush theBrush(traceColor());

    painter.setOpacity(qreal(traceOpacity())/100);

    for( seismic::Gather::size_type idx=firstTrace; idx<= lastTrace; idx++){

        const seismic::Trace& trc=gather[idx];

        qreal x=(idx+0.5)*pixelPerTrace;

        if( m_highlightTrace && idx==m_highlightedTrace ){
            thePen.setColor(Qt::red);
            theBrush.setColor(Qt::red);
        }
        else{
            thePen.setColor(traceColor());
            theBrush.setColor(traceColor());
        }

        painter.save();

        painter.translate( x, (trc.ft()-ft)*pixelPerSecond);
        painter.scale( pixelPerTrace/2 * traceScaleFactors[idx], pixelPerSecond * trc.dt() );
        if( m_variableArea){
            painter.fillPath( m_traceVariableAreaPaths[idx], theBrush);
        }
        if( m_wiggles){
            painter.strokePath(m_traceWigglePaths[idx], thePen);
        }

        painter.restore();

    }

    painter.setOpacity(1);
   m_dirty=true;

        //setUpdatesEnabled(true)
}

/* ORIGINAL
void GatherLabel::paintEvent(QPaintEvent *event){


    Q_ASSERT( m_view );

    if( !m_view->gather() || m_view->gather()->size()<1) return;

    //setUpdatesEnabled(false);

    auto start = std::chrono::steady_clock::now();

    const seismic::Gather& gather=*m_view->gather();

    QRect rect=event->rect();
    int firstTrace=rect.left()/m_view->pixelPerTrace() - 1;
    if( firstTrace<0) firstTrace=0;
    int lastTrace=rect.right()/m_view->pixelPerTrace() + 1;
    if(lastTrace>=gather.size()) lastTrace=gather.size()-1;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    qreal pixelPerTrace=m_view->pixelPerTrace();
    qreal pixelPerSecond=m_view->pixelPerSecond();
    qreal ft=m_view->ft();

    QPen thePen(Qt::black,0);
    QBrush theBrush(Qt::black);

    for( seismic::Gather::size_type idx=firstTrace; idx<= lastTrace; idx++){

        const seismic::Trace& trc=gather[idx];

        qreal x=(idx+0.5)*pixelPerTrace;

        if( idx==m_highlightedTrace ){
            thePen.setColor(Qt::red);
            theBrush.setColor(Qt::red);
        }
        else{
            thePen.setColor(Qt::black);
            theBrush.setColor(Qt::black);
        }

        painter.save();

        painter.translate( x, (trc.ft()-ft)*pixelPerSecond);
        painter.scale( pixelPerTrace/2 * traceScaleFactors[idx], pixelPerSecond * trc.dt() );
        painter.fillPath( m_traceVariableAreaPaths[idx], theBrush);
        painter.strokePath(m_traceWigglePaths[idx], thePen);

        painter.restore();
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout<<"Painting took "<< std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    //setUpdatesEnabled(true);
}

*/
void GatherLabel::resizeEvent(QResizeEvent *){

    m_dirty=true;       // need to update pixmap
    update();
}



qreal lin_inter_y( qreal x1, qreal y1, qreal x2, qreal y2, qreal x){
    qreal dx=x2-x1;
    if( !dx ) return 0.5*(y1+y2);
    return y1 + ( y2 - y1 ) * ( x - x1)/dx;
}

void GatherLabel::updateTracePaths(){


    m_traceWigglePaths.clear();
    m_traceVariableAreaPaths.clear();

    Q_ASSERT( m_view );

    if( !m_view->gather() || m_view->gather()->size()<1) return;

    const seismic::Gather& gather=*m_view->gather();

    m_traceWigglePaths.reserve(gather.size());
    m_traceVariableAreaPaths.reserve( gather.size());

    for( seismic::Gather::size_type i=0; i<gather.size(); i++){

        const seismic::Trace::Samples& samples=gather[i].samples();

        QPainterPath wiggles;

        wiggles.moveTo( samples[0], 0 );
        for( seismic::Trace::size_type j=0; j<samples.size(); j++ ){
            wiggles.lineTo( samples[j], j);
        }


        QPainterPath va;

        QPolygonF polygon;
        qreal lastx=samples[0];
        qreal lasty=0;
        if( samples[0]>0){
            polygon<<QPointF( lastx, lasty);
        }
        for( size_t j=0; j<samples.size(); j++){

            qreal x=samples[j];
            qreal y=j;
             if( polygon.size()>0){

                if( x>0 ){
                    polygon<<QPointF( x, y);
                }
                else{
                    polygon<<QPointF( 0, lin_inter_y(lastx, lasty, x, y, 0) );
                    va.addPolygon( polygon );
                    polygon.clear();
                }
             }
             else{

                 if( x>0){
                    polygon<<QPointF( 0, lin_inter_y(lastx, lasty, x, y, 0)  );
                    polygon<<QPointF( x, y );
                 }

             }

             lastx=x;
             lasty=y;
        }
        if( polygon.size()>0){
            polygon<<QPointF( 0, lasty);
            va.addPolygon( polygon );
        }

        m_traceWigglePaths.push_back(wiggles);
        m_traceVariableAreaPaths.push_back(va);

    }

}

void GatherLabel::updateDensityPlot(){

    Q_ASSERT( m_view );

    if( !m_view->gather() || m_view->gather()->size()<1) return;

    const seismic::Gather& gather=*m_view->gather();
    const ColorTable* colorTable=m_colorTable;

    QImage image( gather.size(), gather.front().samples().size(), QImage::Format_ARGB32 );

    int a=255;
    if( m_view->volume()){
        int opacity=100 - volumeOpacity();
        a=255*opacity/100;
    }
    for( size_t j=0; j<gather.size(); j++){

        const seismic::Trace::Samples& samples=gather[j].samples();
        qreal factor=traceScaleFactors[j];
        for( size_t i=0; i<samples.size(); i++ ){
/*
            qreal amp=127 + 128*factor*samples[i];
            if( amp<0) amp=0;
            if( amp>255 ) amp=255;
*/
            ColorTable::color_type color=colorTable->map(factor * samples[i]);

            image.setPixel( j, i, color);//qRgba( qRed(color), qGreen(color), qBlue(color), a ) );// color);

        }

    }

    m_densityPlot=QPixmap::fromImage(image);

}

void GatherLabel::updateVolumePixmap(){

    if(!m_view || !m_view->volume()) return;

    if( !m_view->gather() || m_view->gather()->size()<1) return;

    const Grid3D<float>& volume=*(m_view->volume());
    const seismic::Gather& gather=*m_view->gather();
    const ColorTable* colorTable=volumeColorTable();

    QImage image( gather.size(), volume.bounds().sampleCount(), QImage::Format_ARGB32);
    int a=255;
    if( m_view->volume()){
        int opacity=volumeOpacity();
        a=255*opacity/100;
    }
//std::cout<<"processing traces...count="<<gather.size()<<std::endl<<std::flush;
//std::cout<<"bounds: il="<<volume.bounds().inline1()<<"-"<<volume.bounds().inline2()<<" xl="<<volume.bounds().crossline1()<<"-"<<volume.bounds().crossline2()<<std::endl<<std::flush;
    for( size_t j=0; j<gather.size(); j++){

        const seismic::Trace& trace=gather[j];
        const seismic::Trace::Samples& samples=trace.samples();
        int iline=trace.header().at("iline").intValue();
        int xline=trace.header().at("xline").intValue();
//std::cout<<"trace="<<j<<" iline="<<iline<<" xline="<<xline<<std::endl<<std::flush;
        if( !volume.bounds().isInside(iline, xline)){
            //std::cout<<"OUT OF VOLUME : il="<<iline<<" xl="<<xline<<std::endl<<std::flush;
            continue;
        }
        for( size_t i=0; i<volume.bounds().sampleCount(); i++ ){


            float value=volume(iline, xline, i);

            /// XXX !!!
            ColorTable::color_type color=colorTable->map(value);
             image.setPixel( j, i, color);//qRgba(qRed(color), qGreen(color), qBlue(color), a) );

        }

    }

    m_volumePixmap=QPixmap::fromImage(image);

}

void GatherLabel::updateBuffers(){

    if( m_density ){
        updateDensityPlot();
    }

    if( m_wiggles || m_variableArea){
        updateTracePaths();
    }

    updateVolumePixmap();  // will do nothing if no volume loaded

     m_dirty=true;   // will update pixmap from paintevent
     update();
}
