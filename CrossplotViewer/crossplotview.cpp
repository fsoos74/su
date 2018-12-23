#include "crossplotview.h"

#include<QProgressDialog>
#include<QApplication>
#include<QGraphicsEllipseItem>
#include<QGraphicsRectItem>
#include<QGraphicsLineItem>
#include<QGraphicsItemGroup>
#include<QMap>
#include<iostream>

namespace{
QMap<CrossplotView::Symbol, QString> slookup{
    {CrossplotView::Symbol::Circle, "Circle"},
    {CrossplotView::Symbol::Square, "Square"},
    {CrossplotView::Symbol::Cross, "Cross"}
};
}

QString toQString(CrossplotView::Symbol s){
    return slookup.value(s);
}

CrossplotView::Symbol toSymbol(QString s){
    return slookup.key(s, CrossplotView::Symbol::Circle);
}

namespace  {
QMap<CrossplotView::ColorStyle, QString> clookup{
    {CrossplotView::ColorStyle::Attribute, "Attribute"},
    {CrossplotView::ColorStyle::Dataset, "Dataset"},
    {CrossplotView::ColorStyle::Fixed, "Fixed"}
};
}

QString toQString(CrossplotView::ColorStyle cs){
    return clookup.value(cs);
}

CrossplotView::ColorStyle toColorStyle(QString s){
    return clookup.key(s, CrossplotView::ColorStyle::Fixed);
}


CrossplotView::CrossplotView(QWidget* parent):RulerAxisView(parent),m_colorTable(new ColorTable(this))
{
    m_filter=Filter{ std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),
                     std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(),
                     0,std::numeric_limits<int>::max()};

    m_colorTable->setColors(ColorTable::defaultColors());

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refresh()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refresh()) );    // NEED TO ADD PRIVATE SLOT
}


void CrossplotView::setData(Crossplot::Data data){

    m_data=data;

    scanBounds();

    scanData();

    refreshScene();

    xAxis()->setRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setRange(m_yRange.minimum, m_yRange.maximum);
    xAxis()->setViewRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setViewRange(m_yRange.minimum, m_yRange.maximum);

    m_colorTable->setRange(m_attributeRange.minimum, m_attributeRange.maximum);  // this triggers refreshScene



    refreshScene();
}


void CrossplotView::setFilter(Filter f){
    m_filter=f;
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

void CrossplotView::setPointSymbol(Symbol s){
    if( s==m_pointSymbol) return;
    m_pointSymbol=s;
    refreshScene();
}

void CrossplotView::setColorStyle(CrossplotView::ColorStyle cs){
    if(cs==m_colorStyle) return;
    m_colorStyle=cs;
    refreshScene();
}

void CrossplotView::setPointSize(int size){

    if( size==m_pointSize) return;

    m_pointSize=size;

    setSelectionPolygon(QPolygonF()); // clear selection

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

    QProgressDialog progress(this);
    progress.setWindowTitle("Crossplot");
    progress.setRange(0,100);
    progress.setLabelText("Populating Crossplot");
    progress.show();
    int perc=0;
    auto const& PS=m_pointSize;

    for( int i=0; i<m_data.size(); i++){

        auto p = m_data[i];

        // check if point is within the inline/crossline/time selection for display
        int msec=static_cast<int>(1000*p.time);
        if( !m_filter.isInside(p.iline,p.xline, msec, p.attribute, p.dataset)) continue;

        QColor color;
        switch (m_colorStyle) {
        case ColorStyle::Fixed:
            color=m_pointColor;
            break;
        case ColorStyle::Attribute:
            color=m_colorTable->map(p.attribute);
            break;
        case ColorStyle::Dataset:
            //color=dscolors[p.dataset%dscolors.size()];
            color=datasetColor(p.dataset);
            break;
        }

        // NULL values are filtered before the data is set, thus they are not checked for
        QPointF pt(p.x, p.y);

        QGraphicsItem* item=nullptr;

        switch(m_pointSymbol){

        case Symbol::Circle:{
                             auto it=new QGraphicsEllipseItem(-PS/2,-PS/2,PS,PS);
                             it->setBrush(QBrush(color));
                             it->setPen(Qt::NoPen);
                             item=it;
                             break;
        }
        case Symbol::Square:{
                             auto it=new QGraphicsRectItem(-PS/2,-PS/2,PS,PS);
                             it->setBrush(QBrush(color));
                             it->setPen(Qt::NoPen);
                             item=it;
                             break;
        }
        case Symbol::Cross:{
                            auto it1=new QGraphicsLineItem(-PS/2,0,PS/2,0);
                            auto it2=new QGraphicsLineItem(0,-PS/2,0,PS/2,it1);
                            QPen pen(color,1);
                            pen.setCosmetic(true);
                            it1->setPen(pen);
                            it2->setPen(pen);
                            item=it1;
                            break;
        }

        }// switch


        item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);

        if( m_flattenTrend){
            qreal dy=m_trend.x() + pt.x()*m_trend.y();
            pt.setY(pt.y()-dy);
        }

        item->setPos( toScene(pt) );  

        // add user data inline and crossline
        item->setData( DATA_INDEX_KEY, i);
        scene->addItem(item);

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

/* original 10.3.18
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
*/

void CrossplotView::scanBounds(){

    // better add progress dialog

    int minil=std::numeric_limits<int>::max();
    int maxil=std::numeric_limits<int>::min();
    int minxl=std::numeric_limits<int>::max();
    int maxxl=std::numeric_limits<int>::min();
    float mint=std::numeric_limits<float>::max();
    float maxt=std::numeric_limits<float>::lowest();

    for( Crossplot::DataPoint point:m_data){
        if(point.iline<minil) minil=point.iline;
        if(point.iline>maxil) maxil=point.iline;
        if(point.xline<minxl) minxl=point.xline;
        if(point.xline>maxxl) maxxl=point.xline;
        if(point.time<mint) mint=point.time;
        if(point.time>maxt) maxt=point.time;
    }

    m_dims=VolumeDimensions(minil, maxil, minxl, maxxl, static_cast<int>(1000*mint), static_cast<int>(1000*maxt));
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
