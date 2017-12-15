#include "volumeview.h"

#include<QGraphicsPixmapItem>
#include<QImage>
#include<iostream>
#include<cmath>


namespace  {

    QMap<VolumeView::SliceType, QString> sliceTypeLookup{

        { VolumeView::SliceType::Inline, "Inline" },
        { VolumeView::SliceType::Crossline, "Crossline"},
        { VolumeView::SliceType::Z, "Z"}
    };

}

QString VolumeView::toQString(SliceType t){
    return sliceTypeLookup.value(t, "Inline");
}

VolumeView::SliceType VolumeView::toSliceType(QString s){
    return sliceTypeLookup.key(s, VolumeView::SliceType::Inline);
}


bool operator==(const VolumeView::SliceDef& a, const VolumeView::SliceDef& b){
    return a.type==b.type && a.value==b.value;
}


VolumeView::VolumeView(QWidget* parent):RulerAxisView(parent), m_flattenRange(0.,0.)
{
    updateBounds();
    refreshScene();
}


QStringList VolumeView::volumeList(){
    return m_volumes.keys();
}

std::shared_ptr<Volume> VolumeView::volume(QString name){
    if( !m_volumes.contains(name)) return std::shared_ptr<Volume>();
    auto vitem=m_volumes.value(name);
    return vitem.volume;
}

qreal VolumeView::volumeAlpha(QString name){
    if( !m_volumes.contains(name)) return 0;
    auto vitem=m_volumes.value(name);
    return vitem.alpha;
}


ColorTable* VolumeView::volumeColorTable(QString name){
    if( !m_volumes.contains(name)) return nullptr;
    auto vitem=m_volumes.value(name);
    return vitem.colorTable;
}

QColor VolumeView::horizonColor(QString name){

    if( !m_horizons.contains(name)) return QColor();

    auto hi=m_horizons.value(name);
    return hi.color;
}

QStringList VolumeView::markersList(){
    QSet<QString> all;
    foreach( std::shared_ptr<WellMarkers> wms, m_markers){
        if( !wms) continue;
        foreach( WellMarker m, *wms){
            all.insert(m.name());
        }
    }

    return QStringList::fromSet(all);
}

void VolumeView::setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy){
    m_xy_to_ilxl=xy_to_ilxl;
    m_ilxl_to_xy=ilxl_to_xy;
    refreshScene();
}


void VolumeView::setSlice(SliceType t, int v){

    setSlice(SliceDef{t,v});
}


bool VolumeView::validSlice(const SliceDef& d){

    bool valid=false;

    auto bounds=m_bounds;
    switch(d.type){
    case SliceType::Inline:
        valid=(d.value>=bounds.i1()) && (d.value<=bounds.i2());
        break;
    case SliceType::Crossline:
        valid=(d.value>=bounds.j1()) &&(d.value<=bounds.j2());
        break;
    case SliceType::Z:
        valid=(d.value>=1000*bounds.ft()) && (d.value<=1000*bounds.lt());
        break;
    }

    return valid;
}

int VolumeView::adjustToVolume(SliceType t, int v){

    auto bounds=m_bounds;

    switch(t){
    case SliceType::Inline:
        if( v<bounds.i1()) v=bounds.i1();
        if( v>bounds.i2()) v=bounds.i2();
        break;
    case SliceType::Crossline:
        if( v<bounds.j1()) v=bounds.j1();
        if( v>bounds.j2()) v=bounds.j2();
        break;
    case SliceType::Z:
        if( v<1000*bounds.ft()) v=1000*bounds.ft();
        if( v>1000*bounds.lt()) v=1000*bounds.lt();
        break;
    }

    return v;
}

void VolumeView::setSlice(SliceDef d){

    const int MAX_HISTORY=1000;

    if( d==m_slice) return;

    if( !validSlice(d)) return;

    m_sliceList.push_back(m_slice);
    if( m_sliceList.size()>MAX_HISTORY) m_sliceList.pop_front();

    auto oldZMinView=zAxis()->minView();
    auto oldZMaxView=zAxis()->maxView();
    auto oldType=m_slice.type;

    m_slice=d;
    updateAxes();
    refreshScene();

    // keep vertical zoom if switching between inlines and crosslines
    if( (oldType==SliceType::Inline || oldType==SliceType::Crossline) &&
            (d.type==SliceType::Inline || d.type==SliceType::Crossline) ) zAxis()->setViewRange(oldZMinView, oldZMaxView);

    emit sliceChanged(d);
}


void VolumeView::back(){
    if(m_sliceList.isEmpty()) return;
    SliceDef slice=m_sliceList.back();
    m_sliceList.pop_back();
    setSlice(slice);
    if(!m_sliceList.isEmpty()) m_sliceList.pop_back();
}

void VolumeView::setWellViewDist(int d){
    if( d==m_wellViewDist) return;
    m_wellViewDist=d;
    refreshScene();
}

void VolumeView::setWellColor(QColor c){
    if( c==m_wellColor) return;
    m_wellColor=c;
    refreshScene();
}

void VolumeView::setMarkerColor(QColor c){
    if( c==m_markerColor) return;
    m_markerColor=c;
    refreshScene();
}


void VolumeView::setLastViewedColor(QColor c){
    if(c==m_lastViewedColor) return;
    m_lastViewedColor=c;
    refreshScene();
}

void VolumeView::setDisplayHorizons(bool on){
    if( on==m_displayHorizons ) return;
    m_displayHorizons=on;
    refreshScene();
}

void VolumeView::setDisplayWells(bool on){
    if(on==m_displayWells ) return;
    m_displayWells=on;
    refreshScene();
}

void VolumeView::setDisplayMarkers(bool on){
    if(on==m_displayMarkers ) return;
    m_displayMarkers=on;
    refreshScene();
}

void VolumeView::setDisplayMarkerLabels(bool on){
    if(on==m_displayMarkerLabels ) return;
    m_displayMarkerLabels=on;
    refreshScene();
}

void VolumeView::setDisplayedMarkers(QStringList l){
    m_displayedMarkers=l;
    refreshScene();
}

void VolumeView::setDisplayLastViewed(bool on){
    if( on==m_displayLastViewed) return;
    m_displayLastViewed=on;
    refreshScene();
}


void VolumeView::addVolume(QString name, std::shared_ptr<Volume> v){

    if( name.isEmpty()) return;
    if( !v ) return;

    auto r=v->valueRange();
    ColorTable* ct=new ColorTable(this, ColorTable::defaultColors(), r );
    if( !ct ) throw std::runtime_error("Allocating colortable failed!");
    connect(ct, SIGNAL(colorsChanged()), this, SLOT(refreshSceneCaller()));
    connect(ct, SIGNAL(powerChanged(double)), this, SLOT(refreshSceneCaller()));
    connect(ct, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshSceneCaller()));

    VolumeItem vitem{ v, 1, ct };

    m_volumes.insert(name, vitem);

    updateBounds();

    refreshScene();

    emit volumesChanged();
}

void VolumeView::removeVolume(QString name){
    if( name.isEmpty()) return;
    if( !m_volumes.contains(name)) return;

    auto vitem=m_volumes.value(name);
    if( vitem.colorTable) delete vitem.colorTable;

    m_volumes.remove(name);

    updateBounds();

    refreshScene();

    emit volumesChanged();
}

void VolumeView::setVolumeAlpha(QString name, double a){
    if( !m_volumes.contains(name)) return;
    auto vitem=m_volumes.value(name);
    if(a==vitem.alpha) return;
    vitem.alpha=a;
    m_volumes[name]=vitem;
    refreshScene();
}

void VolumeView::addHorizon(QString name, std::shared_ptr<Grid2D<float>> grid, QColor color){
    if(m_horizons.contains(name)) return;  // no dupes
    if(!grid) return;
    m_horizons.insert(name, HorizonItem{grid, color});
    refreshScene();
}

void VolumeView::removeHorizon(QString name){
    m_horizons.remove(name);
    refreshScene();
}

void VolumeView::setHorizonColor(QString name, QColor color){
    if( !m_horizons.contains(name)) return;
    auto hitem=m_horizons.value(name);
    hitem.color=color;
    m_horizons[name]=hitem;
    refreshScene();
}

QStringList VolumeView::horizonList(){
    return m_horizons.keys();
}


void VolumeView::addWell(QString name, std::shared_ptr<WellPath> pt, std::shared_ptr<WellMarkers> wms){
    if(m_wells.contains(name)) return; // no dupes
    if(!pt) return;
    m_wells.insert(name, pt);
    m_markers.insert(name, wms);

    refreshScene();
}

void VolumeView::removeWell(QString name){
    m_wells.remove(name);
    m_markers.remove(name);
    refreshScene();
}

QStringList VolumeView::wellList(){
    return m_wells.keys();
}


void VolumeView::setFlattenHorizon(std::shared_ptr<Grid2D<float> > h){

    if( h==m_flattenHorizon) return;
    m_flattenHorizon=h;
    if( m_flattenHorizon){
        m_flattenRange=valueRange(*m_flattenHorizon);
    }
    else{
        m_flattenRange=std::make_pair(0.,0.);
    }
    updateBounds();
    refreshScene();
}


double VolumeView::dz(int i, int j)const{

    if( !m_flattenHorizon ){
        return 0;
    }
    else{
        auto dz=m_flattenHorizon->valueAt(i,j);
        return ( dz!=m_flattenHorizon->NULL_VALUE) ? 0.001*dz : std::numeric_limits<double>::quiet_NaN();
    }
}

QImage VolumeView::intersectVolumeInline(const Volume &volume, ColorTable* colorTable, int iline, double ft, double lt){

    auto bounds=volume.bounds();
    auto dt=bounds.dt();
    auto nt=static_cast<int>(std::round((lt-ft)/dt)+1);
    QImage img( bounds.nj(), nt, QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = 0; i<bounds.nj(); i++){
        auto xl=bounds.j1()+i;
        auto d=dz(iline, xl);
        if( std::isnan(d)) continue;

        for( auto j=0; j<nt; j++){
            auto z=ft + j*dt + d;
            auto value=volume.value(iline, xl, z);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}


QImage VolumeView::intersectVolumeCrossline(const Volume &volume, ColorTable* colorTable, int xline, double ft, double lt){

    auto bounds=volume.bounds();
    auto dt=bounds.dt();
    auto nt=static_cast<int>(std::round((lt-ft)/dt)+1);
    QImage img( bounds.ni(), nt, QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = 0; i<bounds.ni(); i++){
        auto il=bounds.i1()+i;
        auto d=dz(il, xline);
        if( std::isnan(d)) continue;

        for( auto j=0; j<nt; j++){
            auto z=ft + j*dt + d;
            auto value=volume.value(il, xline, z);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}

QImage VolumeView::intersectVolumeTime(const Volume &volume, ColorTable* colorTable, int ms){

    auto bounds=volume.bounds();
    QImage img( bounds.ni(), bounds.nj(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = 0; i<bounds.ni(); i++){
        auto il=bounds.i1()+i;

        for( auto j=0; j<bounds.nj(); j++){
            int xl=bounds.j1()+j;

            auto d=dz(il, xl);
            if( std::isnan(d)) continue;
            auto z=0.001*ms + d;
            auto value=volume.value(il, xl, z);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}


/* ORIGINAL
QImage VolumeView::intersectVolumeInline(const Volume &volume, ColorTable* colorTable, int iline){

    auto bounds=volume.bounds();
    QImage img( bounds.nj(), bounds.nt(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = 0; i<bounds.nj(); i++){
        auto xl=bounds.j1()+i;

        for( auto j=0; j<bounds.nt(); j++){
            auto value=volume.valueAt(iline, xl, j);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}

QImage VolumeView::intersectVolumeCrossline(const Volume &volume, ColorTable* colorTable, int xline){

    auto bounds=volume.bounds();
    QImage img( bounds.ni(), bounds.nt(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = 0; i<bounds.ni(); i++){
        auto il=bounds.i1()+i;

        for( auto j=0; j<bounds.nt(); j++){
            auto value=volume.valueAt(il, xline, j);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}

QImage VolumeView::intersectVolumeTime(const Volume &volume, ColorTable* colorTable, int ms){

    auto bounds=volume.bounds();
    QImage img( bounds.ni(), bounds.nj(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set
    auto k=bounds.timeToSample(0.001*ms);

    for( auto i = 0; i<bounds.ni(); i++){
        auto il=bounds.i1()+i;

        for( auto j=0; j<bounds.nj(); j++){
            int xl=bounds.j1()+j;

            auto value=volume.valueAt(il, xl, k);
            if( value==volume.NULL_VALUE) continue;
            auto col = colorTable->map(value);
            img.setPixel( i, j, col );
       }
    }

    return img;
}

*/


QPainterPath VolumeView::intersectHorizonInline(const Grid2D<float>& grid, int iline){

    QPainterPath path;

    auto bounds=grid.bounds();
    bool isDown=false;

    for( int xline=bounds.j1(); xline<=bounds.j2(); xline++){

        auto p=grid.valueAt(iline,xline);  // returns NULL for non existent location

        if( p==grid.NULL_VALUE){
            isDown=false;
            continue;
        }

        auto d=dz(iline, xline);
        if( std::isnan(d)){
            isDown=false;
            continue;
        }

        p-=1000*d;

        if(isDown){
            path.lineTo(xline, p);
        }
        else{
            path.moveTo(xline, p);
        }

        isDown=true;

    }

    return path;
}


QPainterPath VolumeView::intersectHorizonCrossline(const Grid2D<float>& grid, int xline){

    QPainterPath path;

    auto bounds=grid.bounds();
    bool isDown=false;

    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        auto p=grid.valueAt(iline,xline);   // returns NULL for non existent location

        if( p==grid.NULL_VALUE){
            isDown=false;
            continue;
        }

        auto d=dz(iline, xline);
        if( std::isnan(d)){
            isDown=false;
            continue;
        }

        p-=1000*d;

        if(isDown){
            path.lineTo(iline, p);
        }
        else{
            path.moveTo(iline, p);
        }

        isDown=true;

    }

    return path;
}


QPainterPath VolumeView::projectWellPathInline(const WellPath& wp, int iline){

    QPainterPath path;
    bool isDown=false;

    for( int i=0; i<wp.size(); i++){

        auto p=wp[i];
        auto ilxl=m_xy_to_ilxl.map( QPointF(p.x(), p.y() ) );
        auto xl=ilxl.y(); // static_cast<int>(std::round(ilxl.y()));

        if( std::fabs(ilxl.x()-iline)>m_wellViewDist){
            isDown=false;
            continue;
        }

        auto d=dz(iline, xl);
        if( std::isnan(d)){
            isDown=false;
            continue;
        }
        auto z=-p.z()-1000*d;

        if(isDown){
            path.lineTo(xl, z);
        }
        else{
            path.moveTo(xl, z);
        }

        isDown=true;
    }

    return path;
}

QPainterPath VolumeView::projectWellPathCrossline(const WellPath& wp, int xline){

    QPainterPath path;
    bool isDown=false;

    for( int i=0; i<wp.size(); i++){

        auto p=wp[i];
        auto ilxl=m_xy_to_ilxl.map( QPointF(p.x(), p.y() ) );
        auto il=ilxl.x();// static_cast<int>(std::round(ilxl.x()));

        if( std::fabs(ilxl.y()-xline)>m_wellViewDist){
            isDown=false;
            continue;
        }

        auto d=dz(il, xline);
        if( std::isnan(d)){
            isDown=false;
            continue;
        }

        auto z=-p.z()-1000*d;

        if(isDown){
            path.lineTo(il, z);
        }
        else{
            path.moveTo(il, z);
        }

        isDown=true;
    }

    return path;
}


QVector<std::pair<QPointF, QString>> VolumeView::projectMarkersInline(int iline){

    QVector<std::pair<QPointF, QString>> markers;

    for( auto wname : m_wells.keys()){

        auto wp=m_wells.value(wname);
        if( !wp) continue;
        auto wms=m_markers.value(wname);
        if(!wms) continue;

        for( auto wm : *wms ){
            auto ilxl=m_xy_to_ilxl.map(wp->locationAtMD(wm.md()));
            if( std::fabs(ilxl.x()-iline)>m_wellViewDist) continue;
            auto z=wp->zAtMD(wm.md());
            markers.push_back( std::make_pair(QPointF(ilxl.y(), -z), wm.name()));
         }

    }

    return markers;
}


QVector<std::pair<QPointF, QString>> VolumeView::projectMarkersCrossline(int xline){

    QVector<std::pair<QPointF, QString>> markers;

    for( auto wname : m_wells.keys()){

        auto wp=m_wells.value(wname);
        if( !wp) continue;
        auto wms=m_markers.value(wname);
        if( !wms) continue;

        for( auto wm : *wms ){
            auto ilxl=m_xy_to_ilxl.map(wp->locationAtMD(wm.md()));
            if( std::fabs(ilxl.y()-xline)>m_wellViewDist) continue;
            auto z=wp->zAtMD(wm.md());
            markers.push_back( std::make_pair(QPointF(ilxl.x(), -z), wm.name()));
         }

    }

    return markers;
}


QLineF VolumeView::intersectSlices(const SliceDef &s1, const SliceDef &s2){

    auto bounds=m_bounds;
    QLineF line;

    switch( s1.type){

    case SliceType::Inline:

        switch( s2.type){
        case SliceType::Crossline:
            line= QLineF( s2.value, 1000*bounds.ft(), s2.value, 1000*bounds.lt());
            break;
        case SliceType::Z:
            line=QLineF( bounds.j1(), s2.value, bounds.j2(), s2.value);
            break;
        default:
            break;
        }

        break;

    case SliceType::Crossline:

        switch( s2.type){
        case SliceType::Inline:
            line= QLineF( s2.value, 1000*bounds.ft(), s2.value, 1000*bounds.lt());
            break;
        case SliceType::Z:
            line=QLineF( bounds.i1(), s2.value, bounds.i2(), s2.value);
            break;
        default:
            break;
        }

        break;

    case SliceType::Z:

        switch( s2.type){
        case SliceType::Inline:
            line= QLineF( s2.value, bounds.j1(), s2.value, bounds.j2());
            break;
        case SliceType::Crossline:
            line= QLineF( bounds.i1(), s2.value, bounds.i2(), s2.value);
            break;
        default:
            break;
        }

        break;
    }

    return line;
}

void VolumeView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    renderSlice(scene);

    if( m_displayHorizons) renderHorizons(scene);

    if( m_displayWells) renderWells(scene);

    if( m_displayMarkers) renderMarkers(scene);

    if( m_displayLastViewed) renderLastViewed(scene);

    setScene(scene);
 }


void VolumeView::renderSlice(QGraphicsScene * scene){
    switch(m_slice.type){

    case SliceType::Inline:
        fillSceneInline(scene);
        break;
    case SliceType::Crossline:
        fillSceneCrossline(scene);
        break;
    case SliceType::Z:
        fillSceneTime(scene);
        break;
    }
}

void VolumeView::renderHorizons(QGraphicsScene* scene){
    for( auto hname : m_horizons.keys()){

        auto hi=m_horizons.value(hname);
        QPainterPath path;
        switch(m_slice.type){
        case SliceType::Inline:
            path=intersectHorizonInline(*hi.grid, m_slice.value);
            break;
        case SliceType::Crossline:
            path=intersectHorizonCrossline(*hi.grid, m_slice.value);
            break;
        default:
            break;
        }

        if( !path.isEmpty()){
            auto item=new QGraphicsPathItem(path);
            QPen pen(hi.color, 2);
            pen.setCosmetic(true);
            item->setPen(pen);

            scene->addItem(item);
        }
    }
}

void VolumeView::renderWells(QGraphicsScene * scene){
    QPen wellPen( m_wellColor, 2);
    wellPen.setCosmetic(true);
    for( auto wname : m_wells.keys()){

        auto wp=m_wells.value(wname);
        if( !wp) continue;

        QPainterPath path;   
        switch(m_slice.type){
        case SliceType::Inline:
            path=projectWellPathInline(*wp, m_slice.value);
            break;
        case SliceType::Crossline:
            path=projectWellPathCrossline(*wp, m_slice.value);
            break;
        case SliceType::Z:{
            auto p=m_xy_to_ilxl.map( wp->locationAtZ(-m_slice.value) );
            //std::cout<<"p_ "<<p.x()<<"/"<<p.y()<<std::endl<<std::flush;
            auto item=new QGraphicsEllipseItem( -2,-2, 4, 4);
            item->setPen(wellPen);
            item->setPos(p);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            scene->addItem(item);
            break;
        }
        }

        if( !path.isEmpty()){
            auto item=new QGraphicsPathItem(path);
            item->setPen(wellPen);

            scene->addItem(item);
        }

    }
}

void VolumeView::renderMarkers(QGraphicsScene* scene){

    QVector<std::pair<QPointF,QString>> markers;
    switch(m_slice.type){
    case SliceType::Inline:
        markers=projectMarkersInline(m_slice.value);
        break;
    case SliceType::Crossline:
        markers=projectMarkersCrossline(m_slice.value);
        break;
    default:
        break;
    }

    QPen wmPen(m_markerColor, 2);
    wmPen.setCosmetic(true);
    QFont wmFont("Helvetica [Cronyx]", 8);
    QPen wmLabelPen(m_markerColor,0);

    for( auto marker : markers){

        if( !m_displayedMarkers.contains(marker.second)) continue;

        QGraphicsLineItem* item=new QGraphicsLineItem( -10, 0, 10, 0);
        item->setPen(wmPen);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        item->setPos(marker.first.x(), marker.first.y());
        scene->addItem(item);

        if( m_displayMarkerLabels){
            auto item=new QGraphicsSimpleTextItem(marker.second);
            item->setPen(wmLabelPen);
            item->setFont(wmFont);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            item->setPos(marker.first.x(), marker.first.y() );
            scene->addItem(item);
        }
    }
}

void VolumeView::renderLastViewed(QGraphicsScene * scene){
    if(!m_sliceList.isEmpty()){
        QPen prevPen( m_lastViewedColor, 2);
        prevPen.setCosmetic(true);
        prevPen.setStyle(Qt::DotLine);

        auto prev=m_sliceList.back();
        QLineF l=intersectSlices(m_slice, prev);
        QGraphicsLineItem* item=new QGraphicsLineItem(l);
        item->setPen(prevPen);
        scene->addItem(item);
    }
}

void VolumeView::fillSceneInline(QGraphicsScene * scene){

    if( !scene ) return;

    int il=m_slice.value;

    auto bounds=m_bounds;
    QImage img( bounds.nj(), bounds.nt(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    QPixmap pixmap=QPixmap::fromImage(img);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Plus);

    auto vitems=m_volumes.values();

    for( auto vitem : vitems){
        auto v=vitem.volume;
        auto vbounds=v->bounds();
        auto ft=vbounds.ft()-0.001*m_flattenRange.second;
        auto lt=vbounds.lt()-0.001*m_flattenRange.first;
        auto ct = vitem.colorTable;
        QImage vimg=intersectVolumeInline(*v, ct, il, ft, lt);
        painter.setOpacity(vitem.alpha);
        painter.drawImage( vbounds.j1()-bounds.j1(), bounds.timeToSample(ft), vimg);
    }

    auto item=new QGraphicsPixmapItem();
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPixmap( pixmap );
    scene->addItem(item);

    QTransform tf;
    tf.translate(xAxis()->min(), zAxis()->min());
    tf.scale((xAxis()->max()-xAxis()->min())/img.width(), (zAxis()->max()-zAxis()->min())/img.height());
    item->setTransform(tf);

    scene->setSceneRect(xAxis()->min(), zAxis()->min(), xAxis()->max() - xAxis()->min(), zAxis()->max()-zAxis()->min());
}


void VolumeView::fillSceneCrossline(QGraphicsScene * scene){

    if( !scene ) return;

    int xl=m_slice.value;

    auto bounds=m_bounds;
    QImage img( bounds.ni(), bounds.nt(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    QPixmap pixmap=QPixmap::fromImage(img);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Plus);

    auto vitems=m_volumes.values();

    for( auto vitem : vitems){
        auto v=vitem.volume;
        auto vbounds=v->bounds();
        auto ft=vbounds.ft()-0.001*m_flattenRange.second;
        auto lt=vbounds.lt()-0.001*m_flattenRange.first;
        auto ct = vitem.colorTable;
        QImage vimg=intersectVolumeCrossline(*v, ct, xl, ft, lt);
        painter.setOpacity(vitem.alpha);
        painter.drawImage( vbounds.i1()-bounds.i1(), bounds.timeToSample(ft), vimg);
    }

    auto item=new QGraphicsPixmapItem();
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPixmap( pixmap);
    scene->addItem(item);

    QTransform tf;
    tf.translate(xAxis()->min(), zAxis()->min());
    tf.scale((xAxis()->max()-xAxis()->min())/img.width(), (zAxis()->max()-zAxis()->min())/img.height());
    item->setTransform(tf);
    scene->setSceneRect(xAxis()->min(), zAxis()->min(), xAxis()->max() - xAxis()->min(), zAxis()->max()-zAxis()->min());
}


void VolumeView::fillSceneTime(QGraphicsScene * scene){

    if( !scene ) return;

    auto bounds=m_bounds;
    QImage img( bounds.ni(), bounds.nj(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set


    QPixmap pixmap=QPixmap::fromImage(img);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Plus);

    auto vitems=m_volumes.values();

    for( auto vitem : vitems){
        auto v=vitem.volume;
        auto vbounds=v->bounds();
        auto ct = vitem.colorTable;
        QImage vimg=intersectVolumeTime(*v, ct, m_slice.value);
        painter.setOpacity(vitem.alpha);
        painter.drawImage( vbounds.i1()-bounds.i1(), vbounds.i2()-bounds.i2(), vimg);
    }

    auto item=new QGraphicsPixmapItem();
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setPixmap( pixmap);
    item->setTransformationMode(Qt::SmoothTransformation);
    scene->addItem(item);

    QTransform tf;
    tf.translate(xAxis()->min(), zAxis()->min());
    tf.scale((xAxis()->max()-xAxis()->min())/img.width(), (zAxis()->max()-zAxis()->min())/img.height());
    item->setTransform(tf);

    scene->setSceneRect(xAxis()->min(), zAxis()->min(), xAxis()->max() - xAxis()->min(), zAxis()->max()-zAxis()->min());
}

void VolumeView::updateAxes(){

    auto bounds=m_bounds;

    switch( m_slice.type){
    case SliceType::Inline:
        xAxis()->setRange(bounds.j1(), bounds.j2());
        xAxis()->setName(tr("Crossline"));
        zAxis()->setRange(1000*bounds.ft(), 1000*bounds.lt());
        zAxis()->setName(tr("Time/Depth"));
        break;
    case SliceType::Crossline:
        xAxis()->setRange(bounds.i1(), bounds.i2());
        xAxis()->setName(tr("Inline"));
        zAxis()->setRange(1000*bounds.ft(), 1000*bounds.lt());
        zAxis()->setName(tr("Time/Depth"));
        break;
    case SliceType::Z:
        xAxis()->setRange(bounds.i1(), bounds.i2());
        xAxis()->setName(tr("Inline"));
        zAxis()->setRange(bounds.j1(), bounds.j2());
        zAxis()->setName(tr("Crossline"));
        break;
    }
    zoomFitWindow();
}

void VolumeView::refreshSceneCaller(){
    refreshScene();
}

void VolumeView::updateBounds(){

    if( m_volumes.isEmpty()){
        m_bounds=Grid3DBounds(0,1,0,1,1,0,1);
        return;
    }

    int mini1=std::numeric_limits<int>::max();
    int maxi2=std::numeric_limits<int>::min();
    int minj1=std::numeric_limits<int>::max();
    int maxj2=std::numeric_limits<int>::min();
    qreal minft=std::numeric_limits<qreal>::max();
    qreal maxlt=std::numeric_limits<qreal>::lowest();
    qreal dt=std::numeric_limits<qreal>::max();

    for( auto name : m_volumes.keys()){

        auto v=m_volumes.value(name);
        Q_ASSERT(v.volume);
        auto bounds=v.volume->bounds();

        if( bounds.i1()<mini1 ) mini1=bounds.i1();
        if( bounds.i2()>maxi2 ) maxi2=bounds.i2();
        if( bounds.j1()<minj1 ) minj1=bounds.j1();
        if( bounds.j2()>maxj2 ) maxj2=bounds.j2();
        if( bounds.ft()<minft ) minft=bounds.ft();
        if( bounds.lt()>maxlt ) maxlt=bounds.lt();
        if( bounds.dt()<dt ) dt=bounds.dt();
    }

    int nt=static_cast<int>(std::round( (maxlt-minft)/dt)) +1;
    auto bounds=Grid3DBounds( mini1, maxi2, minj1, maxj2, nt, minft, dt);

    //std::cout<<"IBOUNDS: ft="<<bounds.ft()<<" lt="<<bounds.lt()<<" dt="<<bounds.dt()<<" nt="<<bounds.nt()<<std::endl<<std::flush;

    if( m_flattenHorizon){
        auto rg=valueRange( *m_flattenHorizon);
        std::cout<<"range : "<<rg.first<<" - "<<rg.second<<std::endl;
        auto ft=-0.001*rg.second;
        auto lt=bounds.lt() - 0.001*rg.first;
        auto nt=static_cast<int>(std::round((lt-ft)/bounds.dt())+1);
        //std::cout<<"Bounds ft="<<ft<<" lt="<<lt<<" dt="<<bounds.dt()<<" nt="<<nt<<std::endl<<std::flush;
        bounds=Grid3DBounds(bounds.i1(), bounds.i2(), bounds.j1(), bounds.j2(), nt, ft, bounds.dt());
    }

    m_bounds=bounds;

    updateAxes();
}
