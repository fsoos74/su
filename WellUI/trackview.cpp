#include "trackview.h"

#include<QGraphicsPathItem>
#include<cmath>
#include<iostream>

using namespace std::placeholders;


TrackView::TrackView(QWidget* parent):AxisView(parent)
{

}

qreal TrackView::minZ()const{
    if( !m_log) return 0;
    return log2view(m_log->z0());
}

qreal TrackView::maxZ()const{
    if( !m_log) return 0;
    return log2view(m_log->lz());
}

void TrackView::setLog(std::shared_ptr<Log> l){

    if( l == m_log ) return;

    m_log=l;

    refreshScene();

    //fitInView(sceneRect(), Qt::IgnoreAspectRatio);
}


void TrackView::setWellPath(std::shared_ptr<WellPath> p){

    if( p==m_wellPath) return;
    m_wellPath=p;
    refreshScene();
}


void TrackView::setHighlighted(QVector<qreal> h){
    if( !m_wellPath ) return;

    m_highlighted.clear();
    m_highlighted.reserve(h.size());
    for( auto z : h ){
        auto md=m_wellPath->mdAtZ(z);
        m_highlighted.push_back(md);
    }

    refreshScene();
}


void TrackView::setZFunc(std::function<double (double)> f, std::function<double (double)> invf){
    m_zfunc=f;
    m_izfunc=invf;
    refreshScene();
}


void TrackView::setZShift(qreal s){
    if( s==m_zshift) return;
    m_zshift=s;
    refreshScene();
}

void TrackView::addMarker(Marker m){
    m_markers.insert(m.name, m);
    refreshScene();
}

void TrackView::addMarker(Marker::Type t, QString name, qreal md){
    addMarker(Marker{t, name, md});
}

void TrackView::removeMarker(QString name){
    m_markers.remove(name);
    refreshScene();
}

void TrackView::removeMarkers(Marker::Type t){

    for( auto name : m_markers.keys()){
        if( m_markers.value(name).type==t) m_markers.remove(name);
    }

    refreshScene();
}

void TrackView::removeMarkers(){
    m_markers.clear();
    refreshScene();
}


void TrackView::updateZFunc(){

    if( m_zmode==ZMode::TVD && m_wellPath ){    // if no wellpath defined md=tvd
        setZFunc( std::bind(&WellPath::tvdAtMD, *m_wellPath, _1 ), std::bind(&WellPath::mdAtTVD, *m_wellPath, _1 ) );
    }
    else{
        setZFunc([](double z){return z;}, [](double z){return z;});
    }
}

void TrackView::setFilterLen(int l){
    if( l==m_filterLen) return;
    m_filterLen=l;
    refreshScene();
}

void TrackView::setZMode(ZMode m){
    if(m==m_zmode) return;
    m_zmode=m;
    updateZFunc();
}

Log medianFilter( const Log& log, int len ){

    Log res( "median filtered", log.unit(), "median filtered", log.z0(), log.dz(), log.nz() );

    for( int i=0; i<log.nz(); i++ ){

        int start = std::max( 0, i-len/2);
        int stop = std::min(log.nz()-1, i + len/2 );
        std::vector<double> buf;
        buf.reserve(len);
        for( int i=start; i<=stop; i++){
            if( log[i]==log.NULL_VALUE) continue;
            buf.push_back(log[i]);
        }

        if( buf.size()>0 ){
            std::nth_element( &buf[0], &buf[buf.size()/2], &buf[buf.size()-1] );
            res[i]=buf[buf.size()/2];
        }
        else{
            res[i]=res.NULL_VALUE;
        }
    }

    return res;
}

QPainterPath TrackView::buildPath(Log log){

    QPainterPath path;

    bool isDown=false;

    if( m_filterLen>0) log=medianFilter(log, m_filterLen);

    for( auto i=0; i<log.nz(); i++){

        auto x = log[i];
        if( x==log.NULL_VALUE){
            isDown=false;
            continue;
        }
        auto z = log2view(log.index2z(i));

        x=xAxis()->toScene(x);
        z=zAxis()->toScene(z);

        QPointF p(x,z);
        if( isDown ){
            path.lineTo(p);
        }
        else{
            path.moveTo(p);
            isDown=true;
        }

    }

    return path;
}


qreal TrackView::log2view(qreal z )const{
    auto z0= m_zfunc(0);
    return m_zfunc(z+m_zshift) -z0;  //XXX
}

qreal TrackView::view2log(qreal z)const{
    auto z0= m_zfunc(0);
    return m_izfunc(z+z0)-m_zshift;
}

void TrackView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    if( m_log ){


        // draw log
        auto path=buildPath(*m_log);
        QGraphicsPathItem* gitem=new QGraphicsPathItem(path);
        gitem->setPen( QPen( Qt::darkBlue, 0));
        scene->addItem( gitem );

        // draw highlighted, do this in scene because drawforeground did not work as expected
        // highlighted depths are considered md
        for( auto md : m_highlighted){
            auto md2=md+m_log->dz();
            auto z=zAxis()->toScene(log2view(md));
            auto z2=zAxis()->toScene(log2view(md2));
            //zh+=m_zshift;
            //qreal z=zAxis()->toScene(zh);
            //qreal zh2 = zh + m_log->dz();
            //qreal z2=zAxis()->toScene(zh2);

            QGraphicsRectItem* item=new QGraphicsRectItem(sceneRect().left(), z, sceneRect().width() , z2-z);
            item->setPen( Qt::NoPen );
            item->setBrush( QBrush(qRgba(255,0,0,64) ) );
            item->setOpacity(0.5);
            scene->addItem(item);
        }

        // draw markers
        QPen horizonPen(Qt::blue, 2);
        horizonPen.setCosmetic(true);
        QPen topPen(Qt::darkGreen, 2);
        topPen.setCosmetic(true);
        QFont markerFont("Helvetica [Cronyx]", 10);
        for( auto name : m_markers.keys()){
            auto marker=m_markers.value(name);
            qreal zm=marker.md + m_zshift ;
            qreal z=zAxis()->toScene(zm);

            auto pen=(marker.type==Marker::Type::Horizon) ? horizonPen : topPen;
            QGraphicsLineItem* item=new QGraphicsLineItem(QLineF(sceneRect().left(), z, sceneRect().right(), z) );
            item->setPen(pen);
            item->setOpacity(0.5);
            scene->addItem(item);

            QGraphicsSimpleTextItem* titem=new QGraphicsSimpleTextItem(name);
            titem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            titem->setFont(markerFont);
            titem->setPen(QPen(pen.color(),0));
            titem->setOpacity(0.5);
            scene->addItem(titem);
            titem->setPos(sceneRect().left(), z);
        }

    }

    setScene(scene);
}
