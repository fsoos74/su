#include "scatterplotview.h"

#include<QProgressDialog>
#include<QGraphicsEllipseItem>
#include<QApplication>
#include<iostream>


ScatterplotView::ScatterplotView(QWidget* parent):RulerAxisView(parent)
{
}

QStringList ScatterplotView::keys()const{
    return m_keyColors.keys();
}

QStringList ScatterplotView::displayedKeys()const{
    return m_displayedKeys.toList();
}

QColor ScatterplotView::pointColor(const QString& key)const{
    return m_keyColors.value(key, Qt::black);
}

void ScatterplotView::setDisplayedKeys(QStringList l){

    m_displayedKeys = l.toSet();

    refreshScene();
}

void ScatterplotView::setData(QVector<ScatterPlotPoint> data){

    m_data=data;

    scanData();

    refreshScene();

    xAxis()->setRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setRange(m_yRange.minimum, m_yRange.maximum);
    xAxis()->setViewRange(m_xRange.minimum, m_xRange.maximum);
    zAxis()->setViewRange(m_yRange.minimum, m_yRange.maximum);

    update();
}


void ScatterplotView::setPointSize(int size){

    if( size==m_pointSize) return;

    m_pointSize=size;

    refreshScene();
}


void ScatterplotView::setPointOutline(bool on){

    if( on==m_pointOutline) return;

    m_pointOutline=on;

    refreshScene();
}

void ScatterplotView::setPointColor(QString key, QColor color){

    if( color == pointColor(key)) return;

    m_keyColors[key]=color;

    refreshScene();
}


void ScatterplotView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    if( m_data.size()<1){
        return;
    }

    // ellipse size in pixel, we are scaled to pixel on init and forbit further scaling of items
    int w=m_pointSize;
    int h=m_pointSize;
    QPen thePen=Qt::NoPen;
    if( m_pointOutline){
        thePen=QPen(Qt::black,0);
    }

    QProgressDialog progress(this);
    progress.setWindowTitle("Scatterplot");
    progress.setRange(0,100);
    progress.setLabelText("Populating Scatterplot");
    progress.show();
    int perc=0;

    for( int i=0; i<m_data.size(); i++){

        if( ! m_displayedKeys.contains(m_data[i].key)) continue;

        auto p = m_data[i].coords;
        auto color=pointColor(m_data[i].key);

        auto item = new QGraphicsEllipseItem( -w/2, -h/2, w, h);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        //item->setFlag(QGraphicsItem::ItemIsSelectable, true);

        item->setPos( toScene(p) );
        item->setBrush(color);
        item->setPen(thePen);
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
}


void ScatterplotView::scanData(){

    if(m_data.empty()) return;

    auto minx=m_data[0].coords.x();
    auto maxx=minx;
    auto miny=m_data[0].coords.y();
    auto maxy=miny;

    QSet<QString> keys;

    for( int i=1; i<m_data.size(); i++ ){

        auto x=m_data[i].coords.x();
        if(x<minx) minx=x;
        if(x>maxx) maxx=x;

        auto y=m_data[i].coords.y();
        if(y<miny) miny=y;
        if(y>maxy) maxy=y;

        keys.insert(m_data[i].key);
    }

    m_xRange=Range<double>(minx,maxx);
    m_yRange=Range<double>(miny,maxy);

    // assign initial colors for keys
    m_keyColors.clear();
    int i=0;
    QVector<QColor> icolors{
        Qt::blue, Qt::green, Qt::red, Qt::yellow, Qt::magenta, Qt::cyan, Qt::lightGray,
        Qt::darkBlue, Qt::darkGreen, Qt::darkRed, Qt::darkYellow, Qt::darkMagenta, Qt::darkCyan, Qt::darkGray
    };
    for( auto key : keys){
        m_keyColors[key]=icolors[i%icolors.size()];
        i++;
    }

    m_displayedKeys = keys;
}

void ScatterplotView::refresh(){
    refreshScene();
}
