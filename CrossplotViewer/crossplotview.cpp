#include "crossplotview.h"

#include <datapointitem.h>

#include<QProgressDialog>
#include<QApplication>
#include<iostream>


CrossplotView::CrossplotView(QWidget* parent):RulerAxisView(parent),m_colorTable(new ColorTable(this))
{
    m_colorTable->setColors(ColorTable::defaultColors());

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refresh()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refresh()) );    // NEED TO ADD PRIVATE SLOT
}


void CrossplotView::setData(crossplot::Data data){

    m_data=data;

    scanBounds();

    scanData();

    xAxis()->setRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setRange(m_yRange.minimum, m_yRange.maximum);
    xAxis()->setViewRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setViewRange(m_yRange.minimum, m_yRange.maximum);
    m_colorTable->setRange(m_attributeRange.minimum, m_attributeRange.maximum);  // this triggers refreshScene

    //refreshScene();
}

void CrossplotView::setRegion(VolumeDimensions r){
    m_region=r;
    refreshScene();
}

void CrossplotView::setTrend(QPointF t){

    if( t==m_trend) return;

    m_trend=t;

    // remove selection polygon because it refers to other transform
    // XXX better transform the polygon
    if( isFlattenTrend() ){
        setSelectionPolygon(QPolygonF());
    }

    if( isFlattenTrend() || isDisplayTrendLine() ){ // only refresh if necessary
        refreshScene();
    }
}

void CrossplotView::setFlattenTrend(bool on){

    if( on==isFlattenTrend()) return;

    setSelectionPolygon(QPolygonF());   // clear selection if any

    m_flattenTrend=on;

    refreshScene();
}

void CrossplotView::setDisplayTrendLine(bool on){

    if( on==isDisplayTrendLine()) return;

    m_displayTrendLine=on;

    refreshScene();
}

void CrossplotView::setDatapointSize(int size){

    if( size==m_datapointSize) return;

    m_datapointSize=size;

    setSelectionPolygon(QPolygonF()); // clear selection

    refreshScene();
}

void CrossplotView::setFixedColor(bool fixed){

    if( fixed==m_fixedColor) return;

    m_fixedColor=fixed;

    refreshScene();
}

void CrossplotView::setPointColor(QColor color){

    if( color==m_pointColor) return;

    m_pointColor=color;

    refreshScene();
}


void CrossplotView::setColorMapping( const std::pair<double,double>& m){
    Q_ASSERT( m_colorTable);
    m_colorTable->setRange(m);
}

void CrossplotView::setColors(const QVector<QRgb>& c){
    Q_ASSERT( m_colorTable);
    m_colorTable->setColors(c);
}

void CrossplotView::setTrendlineColor(QColor color){

    if( color==m_trendlineColor) return;

    m_trendlineColor=color;

    if( isDisplayTrendLine()) refreshScene();
}


void CrossplotView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    if( m_data.size()<1){
        return;
    }

    const qreal ACTIVE_SIZE_FACTOR=2;           // datapoints grow by this factor on hoover enter events

    // ellipse size in pixel, we are scaled to pixel on init and forbit further scaling of items
    int w=12;
    int h=12;
    QPen thePen(Qt::black,0);
/*
    qreal minX=std::numeric_limits<qreal>::max();
    qreal maxX=std::numeric_limits<qreal>::lowest();
    qreal minY=std::numeric_limits<qreal>::max();
    qreal maxY=std::numeric_limits<qreal>::lowest();
*/
    QProgressDialog progress(this);
    progress.setWindowTitle("Crossplot");
    progress.setRange(0,100);
    progress.setLabelText("Populating Crossplot");
    progress.show();
    int perc=0;

    for( int i=0; i<m_data.size(); i++){

        crossplot::DataPoint p = m_data[i];

        // check if point is within the inline/crossline/time selection for display
        int msec=static_cast<int>(1000*p.time);
        if( !m_region.isInside(p.iline,p.xline, msec)) continue;


        // NULL values are filtered before the data is set, thus they are not checked for
        QPointF pt(p.x, p.y);

        DatapointItem* item=new DatapointItem( m_datapointSize, ACTIVE_SIZE_FACTOR);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);

        if( m_flattenTrend){
            qreal dy=m_trend.x() + pt.x()*m_trend.y();
            pt.setY(pt.y()-dy);
        }

        item->setPos( toScene(pt) );

        if(m_fixedColor){
            item->setRegularColor( m_pointColor);
        }
        else{
            QRgb rgb=m_colorTable->map(p.attribute);
            item->setRegularColor(rgb);
        }

        // add user data inline and crossline
        item->setData( DATA_INDEX_KEY, i);
        scene->addItem(item);
/*
        if(pt.x()<minX) minX=pt.x();
        if(pt.x()>maxX) maxX=pt.x();
        if(pt.y()<minY) minY=pt.y();
        if(pt.y()>maxY) maxY=pt.y();
*/
        int pp=100*i/m_data.size();
        if(pp>perc){
            perc=pp;
            progress.setValue(perc);
            qApp->processEvents();
            if(progress.wasCanceled()){
                delete scene;
                return;
            }
        }
    }

    setScene(scene);


    if( isDisplayTrendLine()){

        double x1=xAxis()->toAxis(sceneRect().left());
        double x2=xAxis()->toAxis(sceneRect().right());
        double y1=m_trend.x() + x1*m_trend.y();
        double y2=m_trend.x() + x2*m_trend.y();

        if( m_flattenTrend){    // ADJUST THIS TO LOG

            y1=y1 - ( m_trend.x() + x1 *m_trend.y() );
            y2=y2 - ( m_trend.x() + x2 *m_trend.y() );
        }
        std::cout<<"x1="<<x1<<" y1="<<y1<<" x2="<<x2<<" y2="<<y2<<std::endl<<std::flush;
        //m_scene->addLine(x1,y1,x2,y2,QPen(m_trendlineColor, 0));
        QPen trendPen(m_trendlineColor, 2);
        trendPen.setCosmetic(true);
        scene->addLine(QLineF(toScene(QPointF(x1,y1)), toScene(QPointF(x2,y2))), trendPen);
    }

    connect( scene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) ); // forward this to viewer
}

void CrossplotView::scanBounds(){

    // better add progress dialog

    int minil=std::numeric_limits<int>::max();
    int maxil=std::numeric_limits<int>::min();
    int minxl=std::numeric_limits<int>::max();
    int maxxl=std::numeric_limits<int>::min();
    float mint=std::numeric_limits<float>::max();
    float maxt=std::numeric_limits<float>::lowest();

    for( crossplot::DataPoint point:m_data){
        if(point.iline<minil) minil=point.iline;
        if(point.iline>maxil) maxil=point.iline;
        if(point.xline<minxl) minxl=point.xline;
        if(point.xline>maxxl) maxxl=point.xline;
        if(point.time<mint) mint=point.time;
        if(point.time>maxt) maxt=point.time;
    }

    setRegion( VolumeDimensions(minil, maxil, minxl, maxxl, static_cast<int>(1000*mint), static_cast<int>(1000*maxt)));
    //m_geometryBounds=Grid2DBounds(minil, minxl, maxil, maxxl);
    //m_timeRange=Range<float>(mint, maxt);
}

void CrossplotView::scanData(){

    if(m_data.empty()) return;

    float minx=m_data[0].x;
    float maxx=minx;
    float miny=m_data[0].y;
    float maxy=miny;
    float mina=m_data[0].attribute;
    float maxa=mina;

    for( int i=1; i<m_data.size(); i++ ){

        float &x=m_data[i].x;
        if(x<minx) minx=x;
        if(x>maxx) maxx=x;

        float &y=m_data[i].y;
        if(y<miny) miny=y;
        if(y>maxy) maxy=y;

        float &a=m_data[i].attribute;
        if(a<mina) mina=a;
        if(a>maxa) maxa=a;
    }

    m_xRange=Range<float>(minx,maxx);
    m_yRange=Range<float>(miny,maxy);
    m_attributeRange=Range<float>(mina, maxa);
}

void CrossplotView::refresh(){
    refreshScene();
}
