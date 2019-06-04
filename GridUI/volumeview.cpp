#include "volumeview.h"

#include<QGraphicsPixmapItem>
#include<QImage>
#include<iostream>
#include<cmath>
#include<iterator>
#include<volumepicker.h>
#include<matrix.h>
#include"volumeitem.h"
#include"horizonitem.h"
#include"wellitem.h"
#include"markeritem.h"
#include"tableitem.h"
#include"viewitemmodel.h"
#include"imageprocessing.h"
#include"compasswidget.h"

namespace sliceviewer {


bool operator==(const VolumeView::SliceDef& a, const VolumeView::SliceDef& b){
    return a.type==b.type && a.value==b.value;
}

namespace{

double lininterp(double x1, double y1, double x2, double y2, double x){

    // need to add eps check
    if( x1 == x2 ) return (y1+y2)/2;

    return y1 + (x-x1)* ( y2 - y1 ) / ( x2 - x1 );
}

QImage grid2image_column(const Grid1D<float>& g1d, const ColorTable& ct){
    auto b1d=g1d.bounds();
    QImage img(1, b1d.ni(), QImage::Format_ARGB32);
    img.fill(QColor(0,0,0,0));  // transparent
    for(int i=0; i<img.height(); i++){
        for(int j=0; j<img.width(); j++){
            auto value=g1d(i+b1d.i1());
            if(value==g1d.NULL_VALUE) continue;
            auto color=ct.map(value);
            img.setPixel(0,i,color);
        }
    }
    return img;
}

// x==j, y==i
QImage grid2image(const Grid2D<float>& g2d, const ColorTable& ct){
    auto b2d=g2d.bounds();
    QImage img(b2d.nj(), b2d.ni(), QImage::Format_ARGB32);
    img.fill(QColor(0,0,0,0));  // transparent
    for(int i=0; i<img.height(); i++){
        for(int j=0; j<img.width(); j++){
            auto value=g2d(i+b2d.i1(), j+b2d.j1());
            if(value==g2d.NULL_VALUE) continue;
            auto color=ct.map(value);
            img.setPixel(j,i,color);
        }
    }
    return img;
}

// x==i, y==j
QImage grid2image_r(const Grid2D<float>& g2d, const ColorTable& ct){
    auto b2d=g2d.bounds();
    QImage img(b2d.ni(), b2d.nj(), QImage::Format_ARGB32);
    img.fill(QColor(0,0,0,0));  // transparent
    for(int i=0; i<img.width(); i++){
        for(int j=0; j<img.height(); j++){
            auto value=g2d(i+b2d.i1(), j+b2d.j1());
            if(value==g2d.NULL_VALUE) continue;
            auto color=ct.map(value);
            img.setPixel(i,j,color);
        }
    }
    return img;
}

// y=bounds.i1...bounds.i2, x:g1d.valuerange
QPainterPath grid1d2wiggles(const Grid1D<float>& g1d){
    // wiggles
    QPainterPath path;
    auto b1d=g1d.bounds();
    bool first=true;
    for( auto i=b1d.i1(); i<=b1d.i2(); i++){
        auto value=g1d(i);
        if( value==g1d.NULL_VALUE) continue;
        if(first){
            path.moveTo(value,i);
            first=false;
        }
        else{
            path.lineTo(value,i);
        }
    }
    return path;
}

QPainterPath grid1d2va(const Grid1D<float>& g1d){
    auto b1d=g1d.bounds();
    QPainterPath path;
    bool active=false;
    double xprev=0;
    double zprev=0;
    for( auto i=b1d.i1(); i<=b1d.i2(); i++){
        auto z=i;
        auto x=g1d(i);
        if( x==g1d.NULL_VALUE) continue;
        if(x>=0){               // inside va
            if(!active){        // no area yet, start new
                auto zxl=(i>b1d.i1()) ? lininterp(xprev, zprev,x,z,0) : z;
                path.moveTo(0,zxl);
                //path.moveTo(x,z);
                active=true;
            }
            path.lineTo(x,z);   // line to current point
        }
        else if(active){        // close area
            auto zxl=lininterp(xprev, zprev, x, z, 0);
            path.lineTo(0,zxl);
            //path.lineTo(x,z);
            active=false;
            path.closeSubpath();
        }
        xprev=x;
        zprev=z;
    }
    return path;
}

QPainterPath grid1d2hpath(const Grid1D<float>& g1d){

    QPainterPath path;

    auto b1d=g1d.bounds();
    bool isDown=false;

    for( int i=b1d.i1(); i<=b1d.i2(); i++){

        auto value=g1d(i);
        if( value==g1d.NULL_VALUE){
            isDown=false;
            continue;
        }

        if(isDown){
            path.lineTo(i, value);
        }
        else{
            path.moveTo(i, value);
        }

        isDown=true;
    }

    return path;
}

QVector<QPointF> intersectTableIline(const Table &table, int iline){

    auto points=table.atKey1(iline);
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<1>(p), 1000*std::get<2>(p)));   // millis
    }
    return res;
}

QVector<QPointF> intersectTableXline(const Table &table, int xline){
    auto points=table.atKey2(xline);
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<0>(p), 1000*std::get<2>(p)));   // millis
    }
    return res;
}

QVector<QPointF> intersectTableTime(const Table &table, int time, float EPS=0.01f){

    auto points=table.inValueRange(0.001*time-EPS,0.001*time+EPS );           // msec -> sec
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<0>(p), std::get<1>(p)));
    }

    return res;
}

}


VolumeView::VolumeView(QWidget* parent):RulerAxisView(parent),
    mCompassWidget(new CompassWidget(this)),
    m_picker(new VolumePicker(this)),
    mHorizonItemModel(new ViewItemModel(this)),
    mVolumeItemModel(new ViewItemModel(this)),
    mWellItemModel(new ViewItemModel(this)),
    mMarkerItemModel(new ViewItemModel(this)),
    mTableItemModel(new ViewItemModel(this)),
    m_flattenRange(0.,0.)
{
    setCornerWidget(mCompassWidget);
    setMouseMode(MouseMode::Explore);
    connect(mVolumeItemModel,SIGNAL(changed()), this, SLOT(onVolumeItemModelChanged()));
    connect(mVolumeItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mHorizonItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mHorizonItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mWellItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mWellItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mMarkerItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mMarkerItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(xAxis(),SIGNAL(reversedChanged(bool)),this,SLOT(updateCompass()));
    connect(xAxis(),SIGNAL(rangeChanged(double,double)), this, SLOT(updateCompass()));
    connect(zAxis(),SIGNAL(reversedChanged(bool)),this,SLOT(updateCompass()));
    connect(zAxis(),SIGNAL(rangeChanged(double,double)), this, SLOT(updateCompass()));
    updateBounds();
    refreshScene();
}

void VolumeView::setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy){
    m_xy_to_ilxl=xy_to_ilxl;
    m_ilxl_to_xy=ilxl_to_xy;
    refreshScene();
}

void VolumeView::setDisplayOptions(const DisplayOptions & opts){
    bool axeschanged=opts.inlineOrientation()!=mDisplayOptions.inlineOrientation();
    mDisplayOptions=opts;
    setRenderHint(QPainter::RenderHint::Antialiasing, mDisplayOptions.isAntiliasing());
    if(axeschanged){
        updateAxes();
        updateCompass();
        inlineOrientationChanged(opts.inlineOrientation());
    }
    refreshScene();
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

    clearSelection();

    m_sliceList.push_back(m_slice);
    if( m_sliceList.size()>MAX_HISTORY) m_sliceList.pop_front();

    auto oldXMinView=xAxis()->minView();
    auto oldXMaxView=xAxis()->maxView();
    auto oldZMinView=zAxis()->minView();
    auto oldZMaxView=zAxis()->maxView();
    auto oldType=m_slice.type;

    m_slice=d;
    updateAxes();
    updateCompass();
    refreshScene();

    if( d.type==oldType){ // keep zoom if same slice type
        xAxis()->setViewRange(oldXMinView, oldXMaxView);
        zAxis()->setViewRange(oldZMinView, oldZMaxView);
    }
    else if( d.type==SliceType::Z){ // new approach to keep fixed aspect ratio
        setKeepAspectRatio(true);
        zoomFitWindow();
    }
    else if( (oldType==SliceType::Inline || oldType==SliceType::Crossline) &&
             (d.type==SliceType::Inline || d.type==SliceType::Crossline) ){
        // keep vertical zoom if switching between inlines and crosslines
         zAxis()->setViewRange(oldZMinView, oldZMaxView);
     }
    else{
        setKeepAspectRatio(false);
        zoomFitWindow();
    }
    emit sliceChanged(d);
}


void VolumeView::back(){
    if(m_sliceList.isEmpty()) return;
    SliceDef slice=m_sliceList.back();
    m_sliceList.pop_back();
    setSlice(slice);
    if(!m_sliceList.isEmpty()) m_sliceList.pop_back();
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
        return ( dz!=m_flattenHorizon->NULL_VALUE) ? 0.001*dz : 0;
    }
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

    if(mDisplayOptions.isDisplayVolumes()) renderVolumes(scene);

    if( mDisplayOptions.isDisplayHorizons()) renderHorizons(scene);

    if( mDisplayOptions.isDisplayWells()) renderWells(scene);

    if( mDisplayOptions.isDisplayMarkers()) renderMarkers(scene);

    if( mDisplayOptions.isDisplayTables()) renderTables(scene);

    if( mDisplayOptions.isDisplayLastViewed() ) renderLastViewed(scene);

    setScene(scene);
 }


void VolumeView::renderHorizonIline(QGraphicsScene* scene, const HorizonItem& hi, int iline){

    auto g1d=hi.horizon()->atI(iline);
    if(!g1d) return;

    // build path
    QPainterPath path;
    auto b1d=g1d->bounds();
    bool isDown=false;
    for( int i=b1d.i1(); i<=b1d.i2(); i++){
        auto z=(*g1d)(i);
        if( z==g1d->NULL_VALUE){
            isDown=false;
            continue;
        }

        // apply flatening
        z-=1000*dz(iline,i);

        if(isDown){
            path.lineTo(i, z);
        }
        else{
            path.moveTo(i, z);
        }

        isDown=true;
    }

    QPen pen(hi.color(), hi.width());
    pen.setCosmetic(true);
    auto item=new QGraphicsPathItem(path);
    item->setPen(pen);
    item->setOpacity(0.01*hi.opacity());   // percent -> fraction
    scene->addItem(item);
}

void VolumeView::renderHorizonXline(QGraphicsScene* scene, const HorizonItem& hi, int xline){

    auto g1d=hi.horizon()->atJ(xline);
    if(!g1d) return;

    // build path
    QPainterPath path;
    auto b1d=g1d->bounds();
    bool isDown=false;
    for( int i=b1d.i1(); i<=b1d.i2(); i++){
        auto z=(*g1d)(i);
        if( z==g1d->NULL_VALUE){
            isDown=false;
            continue;
        }

        // apply flatening
        z-=1000*dz(i, xline);

        if(isDown){
            path.lineTo(i, z);
        }
        else{
            path.moveTo(i, z);
        }

        isDown=true;
    }

    QPen pen(hi.color(), hi.width());
    pen.setCosmetic(true);
    auto item=new QGraphicsPathItem(path);
    item->setPen(pen);
    item->setOpacity(0.01*hi.opacity());   // percent -> fraction
    scene->addItem(item);
}

void VolumeView::renderHorizons(QGraphicsScene* scene){

    for( int i=mHorizonItemModel->size()-1; i>=0; i--){

        auto hi=dynamic_cast<HorizonItem*>(mHorizonItemModel->at(i));
        if(!hi) continue;

        if(m_slice.type==SliceType::Inline){
            renderHorizonIline(scene, *hi, m_slice.value);
        }
        else if(m_slice.type==SliceType::Crossline){
            renderHorizonXline(scene, *hi, m_slice.value);
        }

    }
}

void VolumeView::renderWells(QGraphicsScene * scene){

    for( int i=mWellItemModel->size()-1; i>=0; i-- ){
        auto witem=dynamic_cast<WellItem*>(mWellItemModel->at(i));
        if(!witem) continue;

        switch(m_slice.type){
        case SliceType::Inline: renderWellIline(scene, *witem, m_slice.value); break;
        case SliceType::Crossline: renderWellXline(scene, *witem, m_slice.value); break;
        case SliceType::Z: renderWellTime(scene, *witem, m_slice.value); break;
        }
    }
}

void VolumeView::renderWellIline(QGraphicsScene * scene, const WellItem& witem, int il){

        auto wp=witem.wellPath();
        if( !wp) return;

        QPainterPath path;
        bool isDown=false;

        for( int i=0; i<wp->size(); i++){

            auto p=(*wp)[i];
            auto ilxl=m_xy_to_ilxl.map( QPointF(p.x(), p.y() ) );

            if( std::fabs(ilxl.x()-il)>mDisplayOptions.wellVisibilityDistance()){
                isDown=false;
                continue;
            }

            auto z=-p.z()-1000*dz(ilxl.x(), ilxl.y());

            if(isDown){
                path.lineTo(ilxl.y(), z);
            }
            else{
                path.moveTo(ilxl.y(), z);
            }

            isDown=true;
        }

        if( !path.isEmpty()){
            QPen wellPen( witem.color(), witem.width());
            wellPen.setCosmetic(true);
            auto item=new QGraphicsPathItem(path);
            item->setPen(wellPen);
            scene->addItem(item);
            auto label=witem.label();
            if(!label.isEmpty()){
                QFont labelFont("Helvetica [Cronyx]", witem.labelFontSize());
                auto item=new QGraphicsTextItem(label);
                item->setPos(path.currentPosition());
                item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                item->setFont(labelFont);
                item->setDefaultTextColor(witem.color());
                scene->addItem(item);
            }
        }
}

void VolumeView::renderWellXline(QGraphicsScene * scene, const WellItem& witem, int xl){

        auto wp=witem.wellPath();
        if( !wp) return;

        QPainterPath path;
        bool isDown=false;

        for( int i=0; i<wp->size(); i++){

            auto p=(*wp)[i];
            auto ilxl=m_xy_to_ilxl.map( QPointF(p.x(), p.y() ) );

            if( std::fabs(ilxl.y()-xl)>mDisplayOptions.wellVisibilityDistance()){
                isDown=false;
                continue;
            }

            auto z=-p.z()-1000*dz(ilxl.x(), ilxl.y());

            if(isDown){
                path.lineTo(ilxl.x(), z);
            }
            else{
                path.moveTo(ilxl.x(), z);
            }

            isDown=true;
        }

        if( !path.isEmpty()){
            QPen wellPen( witem.color(), witem.width());
            wellPen.setCosmetic(true);
            auto item=new QGraphicsPathItem(path);
            item->setPen(wellPen);
            scene->addItem(item);
            auto label=witem.label();
            if(!label.isEmpty()){
                QFont labelFont("Helvetica [Cronyx]", witem.labelFontSize());
                auto item=new QGraphicsTextItem(label);
                item->setPos(path.currentPosition());
                item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                item->setFont(labelFont);
                item->setDefaultTextColor(witem.color());
                scene->addItem(item);
            }
        }
}

void VolumeView::renderWellTime(QGraphicsScene * scene, const WellItem& witem, int t){
    auto wp=witem.wellPath();
    if( !wp) return;

    QTransform tf;
    if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
        tf=swappedIlineXlineTransform();
    }

    auto p=m_xy_to_ilxl.map( wp->locationAtZ(-t) );
    auto s=witem.width();
    auto item=new QGraphicsEllipseItem( -s,-s, 2*s, 2*s);
    item->setPen(Qt::NoPen);
    item->setBrush(witem.color());
    p=tf.map(p);
    item->setPos(p);
    item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    scene->addItem(item);
    auto label=witem.label();
    if(!label.isEmpty()){
        QFont labelFont("Helvetica [Cronyx]", witem.labelFontSize());
        auto item=new QGraphicsTextItem(label);
        item->setPos(p);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        item->setFont(labelFont);
        item->setDefaultTextColor(witem.color());
        scene->addItem(item);
    }
}

void VolumeView::renderMarkers(QGraphicsScene* scene){

    if(m_slice.type==SliceType::Z) return;

    for( int i=mMarkerItemModel->size()-1; i>=0; i--){
        auto mitem=dynamic_cast<MarkerItem*>(mMarkerItemModel->at(i));
        if(!mitem) continue;

        auto mposition=mitem->position();
        auto ilxl=m_xy_to_ilxl.map( QPointF(mposition.x(), mposition.y() ) );   // convert coords to lines
        auto x=(m_slice.type==SliceType::Inline) ? ilxl.y() : ilxl.x();         // use xline for iline slices and vice versa
        auto dist=(m_slice.type==SliceType::Inline) ?                           // distance in lines
                    ilxl.x() - m_slice.value : ilxl.y() -m_slice.value;
        if(std::abs(dist)>mDisplayOptions.wellVisibilityDistance()) continue;   // too far away to be shown
        auto y=-mposition.z();                                                  // position-z-axis points upwards but view y-axis increases downwards
        y-=1000*dz(ilxl.x(),ilxl.y());                                          // apply flatening

        QPen wmPen(mitem->color(), mitem->width());
        wmPen.setCosmetic(true);
        QPen wmLabelPen(mitem->color(),0);
        QGraphicsLineItem* item=new QGraphicsLineItem( -10, 0, 10, 0);
        item->setPen(wmPen);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        item->setPos( x, y);
        scene->addItem(item);

        QString label=mitem->label();
        if(!label.isEmpty()){
            QFont wmFont("Helvetica [Cronyx]", mitem->labelFontSize());
            auto litem=new QGraphicsTextItem(label);
            litem->setDefaultTextColor(mitem->color());
            litem->setFont(wmFont);
            litem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            litem->setPos(x, y );
            scene->addItem(litem);
        }
    }
}

void VolumeView::renderTables(QGraphicsScene* scene){

    for( int i=mTableItemModel->size()-1; i>=0; i--){
        auto titem=dynamic_cast<TableItem*>(mTableItemModel->at(i));
        if(!titem) continue;

        switch(m_slice.type){
        case SliceType::Inline: renderTableIline(scene, *titem, m_slice.value); break;
        case SliceType::Crossline: renderTableXline(scene, *titem, m_slice.value); break;
        case SliceType::Z: renderTableTime(scene,*titem, m_slice.value); break;
        }
    }
}


void VolumeView::renderTableIline(QGraphicsScene* scene, const TableItem& titem, int il){
    auto table=titem.table();
    if(!table) return;
    auto s=titem.pointSize();
    QBrush brush(titem.color());
    for( auto pos : intersectTableIline(*table, il) ){
        auto item=new QGraphicsRectItem( -s/2,-s/2, s, s);
        item->setPen(Qt::NoPen);
        item->setBrush(brush);
        item->setPos(pos);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
    }
}

void VolumeView::renderTableXline(QGraphicsScene* scene, const TableItem& titem, int xl){
    auto table=titem.table();
    if(!table) return;
    auto s=titem.pointSize();
    QBrush brush(titem.color());
    for( auto pos : intersectTableXline(*table, xl) ){
        auto item=new QGraphicsRectItem( -s/2,-s/2, s, s);
        item->setPen(Qt::NoPen);
        item->setBrush(brush);
        item->setPos(pos);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
    }
}

void VolumeView::renderTableTime(QGraphicsScene* scene, const TableItem& titem, int t){
    const float EPS=0.01f;  // z-range around z for table values to be included for z

    auto table=titem.table();
    if(!table) return;
    QTransform tf;
    if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
        tf=swappedIlineXlineTransform();
    }
    auto s=titem.pointSize();
    QBrush brush(titem.color());
    for( auto p : intersectTableTime(*table,t,EPS) ){
        auto pos=tf.map(p);
        auto item=new QGraphicsRectItem( -s/2,-s/2, s, s);
        item->setPen(Qt::NoPen);
        item->setBrush(brush);
        item->setPos(pos);
        item->setTransform(tf);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
    }
}

void VolumeView::drawForeground(QPainter *painter, const QRectF &rectInScene){

    painter->save();
    painter->setWorldMatrixEnabled(false);

    QVector<QPointF> points;
    switch(m_slice.type){
    case SliceType::Inline:
        points=intersectTableIline(*m_picker->picks(), m_slice.value);
        break;
    case SliceType::Crossline:
        points=intersectTableXline(*m_picker->picks(), m_slice.value);
        break;
    case SliceType::Z:
        points=intersectTableTime(*m_picker->picks(), m_slice.value);
        if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
              QTransform tf=swappedIlineXlineTransform();
              for(auto& p : points){
                  p=tf.map(p);
              }
        }
        break;
    default:
        break;
    }

    QPen pickPen(Qt::red,2);
    pickPen.setCosmetic(true);
    painter->setPen(pickPen);
    const int S=3;

    for( auto p:points){

        auto pp=mapFromScene(p);
        painter->drawLine(pp.x()-S, pp.y(), pp.x()+S, pp.y()+S);
        painter->drawLine(pp.x()+S, pp.y(), pp.x()-S, pp.y()+S);
    }

    painter->setWorldMatrixEnabled(true);
    painter->restore();

    AxisView::drawForeground( painter, rectInScene );
}

void VolumeView::renderLastViewed(QGraphicsScene * scene){
    if(!m_sliceList.isEmpty()){
        QPen prevPen( mDisplayOptions.lastViewedColor(), 2);
        prevPen.setCosmetic(true);
        prevPen.setStyle(Qt::DotLine);

        auto prev=m_sliceList.back();
        QLineF l=intersectSlices(m_slice, prev);
        QGraphicsLineItem* item=new QGraphicsLineItem(l);
        item->setPen(prevPen);
        if(m_slice.type==SliceType::Z && mDisplayOptions.inlineOrientation()==Qt::Horizontal){
            item->setTransform(swappedIlineXlineTransform());
        }
        scene->addItem(item);
    }
}

QImage VolumeView::sharpen(QImage src){
    if(!mDisplayOptions.isSharpen()) return src;
    auto kernel=imageprocessing::buildSharpenKernel(mDisplayOptions.sharpenFilterSize(),
                                                    0.01*mDisplayOptions.sharpenFilterStrength());
    return imageprocessing::convolve(src,kernel);
}

void VolumeView::renderVolumes(QGraphicsScene * scene){

    if( !scene ) return;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        switch(m_slice.type){
        case SliceType::Inline: renderVolumeIline(scene, *vitem, m_slice.value); break;
        case SliceType::Crossline: renderVolumeXline(scene, *vitem, m_slice.value); break;
        case SliceType::Z: renderVolumeTime(scene,*vitem, m_slice.value); break;
        }
    }
 }

void VolumeView::renderVolumeIline(QGraphicsScene * scene, const VolumeItem& vitem, int il){

    auto v=vitem.volume();
    Q_ASSERT(v);
    auto vbounds=v->bounds();
    auto ct = vitem.colorTable();
    Q_ASSERT(ct);

    if(vitem.style()==VolumeItem::Style::ATTRIBUTE){   // render as image
        // find range for this line because this matches image, not overall flatten range
        auto mindz=std::numeric_limits<float>::max();
        auto maxdz=std::numeric_limits<float>::lowest();
        for( int xl=vbounds.j1(); xl<=vbounds.j2(); xl++){
            auto d=dz(il,xl);
            if(d<mindz) mindz=d;
            if(d>maxdz) maxdz=d;
        }
        auto nt=static_cast<int>(std::round(vbounds.nt()+(maxdz-mindz)/vbounds.dt()+1));
        QImage vimg(vbounds.nj(),nt,QImage::Format_ARGB32);
        vimg.fill(qRgba(0,0,0,0));
        QPainter painter(&vimg);
        for(int j=0; j<vimg.width(); j++){
            auto xl=vbounds.j1()+j;
            auto g1d=v->atIJ(il,xl);
            if(!g1d) continue;
            auto imgj=grid2image_column(*g1d,*ct);
            auto d=dz(il,xl);
            auto y=(maxdz-d)/vbounds.dt();
            painter.drawImage(j,y,imgj);
        }
        QPixmap pixmap=QPixmap::fromImage(vimg);

        auto item=new QGraphicsPixmapItem();
        item->setTransformationMode(Qt::SmoothTransformation);
        item->setPixmap( pixmap );
        item->setOpacity(0.01*vitem.opacity());    // percent > fraction

        QTransform tf;
        tf.translate(vbounds.j1(),1000*(vbounds.ft()-maxdz));//vbounds.ft());
        tf.scale(1,1000.*vbounds.dt());         // sec -> msec
        item->setTransform(tf);
        scene->addItem(item);
    }
    else{
        if(!mVolumeStatistics.contains(vitem.name())){
            mVolumeStatistics.insert(vitem.name(),
                statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
        }
        auto stats=mVolumeStatistics.value(vitem.name());
        auto tx=-stats.rms;
        auto sx=1./(3*stats.sigma);  // 3 standard deviations
        sx*=vitem.gain();
        if(vitem.polarity()==VolumeItem::Polarity::REVERSED){
            sx*=-1;
        }

        for( auto j = vbounds.j1(); j<=vbounds.j2(); j++){
            auto g1d=v->atIJ(il,j);
            if(!g1d) continue;

            QTransform tf;
            tf.translate(j,1000*(vbounds.ft()-dz(il,j)));
            tf.scale(sx,1000*vbounds.dt());
            tf.translate(tx,0);

            // wiggles
            auto path=grid1d2wiggles(*g1d);
            auto item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(QPen(Qt::black, 0));
            item->setOpacity(0.01*vitem.opacity());  // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);

            // variable area
            path=grid1d2va(*g1d);
            item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(Qt::NoPen);
            item->setBrush(Qt::black);
            item->setOpacity(0.01*vitem.opacity());        // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);
        }
    }
}

void VolumeView::renderVolumeXline(QGraphicsScene * scene, const VolumeItem& vitem, int xl){

    auto v=vitem.volume();
    Q_ASSERT(v);
    auto vbounds=v->bounds();
    auto ct = vitem.colorTable();
    Q_ASSERT(ct);

    if(vitem.style()==VolumeItem::Style::ATTRIBUTE){   // render as image
        // find range for this line because this matches image, not overall flatten range
        auto mindz=std::numeric_limits<float>::max();
        auto maxdz=std::numeric_limits<float>::lowest();
        for( int il=vbounds.i1(); il<=vbounds.i2(); il++){
            auto d=dz(il,xl);
            if(d<mindz) mindz=d;
            if(d>maxdz) maxdz=d;
        }
        auto nt=static_cast<int>(std::round(vbounds.nt()+(maxdz-mindz)/vbounds.dt()+1));
        QImage vimg(vbounds.nj(),nt,QImage::Format_ARGB32);
        vimg.fill(qRgba(0,0,0,0));
        QPainter painter(&vimg);
        for(int j=0; j<vimg.width(); j++){
            auto il=vbounds.i1()+j;
            auto g1d=v->atIJ(il,xl);
            if(!g1d) continue;
            auto imgj=grid2image_column(*g1d,*ct);
            auto d=dz(il,xl);
            auto y=(maxdz-d)/vbounds.dt();
            painter.drawImage(j,y,imgj);
        }
        QPixmap pixmap=QPixmap::fromImage(vimg);

        auto item=new QGraphicsPixmapItem();
        item->setTransformationMode(Qt::SmoothTransformation);
        item->setPixmap( pixmap );
        item->setOpacity(0.01*vitem.opacity());    // percent > fraction

        QTransform tf;
        tf.translate(vbounds.i1(),1000*(vbounds.ft()-maxdz));//vbounds.ft());
        tf.scale(1,1000.*vbounds.dt());         // sec -> msec
        item->setTransform(tf);
        scene->addItem(item);
    }
    else{
        // render as seismic traces
        if(!mVolumeStatistics.contains(vitem.name())){
            mVolumeStatistics.insert(vitem.name(),
                statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
        }
        auto stats=mVolumeStatistics.value(vitem.name());
        auto tx=-stats.rms;
        auto sx=1./(3*stats.sigma);  // 3 standard deviations
        sx*=vitem.gain();
        if(vitem.polarity()==VolumeItem::Polarity::REVERSED){
            sx*=-1;
        }

        for( auto i = vbounds.i1(); i<=vbounds.i2(); i++){
            auto g1d=v->atIJ(i,xl);
            if(!g1d) continue;

            QTransform tf;
            tf.translate(i,1000*(vbounds.ft()-dz(i,xl)));
            tf.scale(sx,1000*vbounds.dt());
            tf.translate(tx,0);

            // wiggles
            auto path=grid1d2wiggles(*g1d);
            auto item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(QPen(Qt::black, 0));
            item->setOpacity(0.01*vitem.opacity());  // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);

            // variable area
            path=grid1d2va(*g1d);
            item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(Qt::NoPen);
            item->setBrush(Qt::black);
            item->setOpacity(0.01*vitem.opacity());        // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);
        }
    }
}


void VolumeView::renderVolumeTime(QGraphicsScene * scene, const VolumeItem& vitem, int t){

    auto v=vitem.volume();
    Q_ASSERT(v);
    auto vbounds=v->bounds();
    auto ct = vitem.colorTable();
    Q_ASSERT(ct);

    auto b2d=Grid2DBounds(vbounds.i1(), vbounds.j1(), vbounds.i2(), vbounds.j2());
    auto times=std::make_unique<Grid2D<double>>(b2d);
    for(int i=b2d.i1(); i<=b2d.i2(); i++){
        for(auto j=b2d.j1(); j<=b2d.j2(); j++){
            (*times)(i,j)=0.001*t+dz(i,j);
        }
    }

    if(vitem.style()==VolumeItem::Style::ATTRIBUTE){   // render as image
        //auto g2d=vitem->volume()->atT(0.001*t);          // msec -> sec
        auto g2d=vitem.volume()->atT(*times);
        if(!g2d) return;
        // apply gain, polarity
        double b=0;
        double m=vitem.gain();
        if(vitem.polarity()==VolumeItem::Polarity::REVERSED){
            b=ct->range().second;
            m*=-1;
        }
        for( auto &x : *g2d){
            x=b+m*x;
        }
        auto vimg=grid2image_r(*g2d,*ct);               // i->x, j->y
        vimg=sharpen(vimg);
        QPixmap pixmap=QPixmap::fromImage(vimg);

        auto item=new QGraphicsPixmapItem();
        item->setTransformationMode(Qt::SmoothTransformation);
        item->setPixmap( pixmap );
        item->setOpacity(0.01*vitem.opacity());    // percent > fraction

        QTransform tf;
        tf.translate(v->bounds().i1(), v->bounds().j1());
        tf.scale(qreal(vbounds.i2()-vbounds.i1())/vimg.width(),
                 qreal(vbounds.j2()-vbounds.j1())/vimg.height());
        if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
            tf*=swappedIlineXlineTransform();
        }
        item->setTransform(tf);
        scene->addItem(item);
    }
    else{
        // render as seismic traces
        if(!mVolumeStatistics.contains(vitem.name())){
            mVolumeStatistics.insert(vitem.name(),
                statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
        }
        auto stats=mVolumeStatistics.value(vitem.name());
        auto tx=-stats.rms;
        auto sx=1./(3*stats.sigma);  // 3 standard deviations
        sx*=vitem.gain();
        if(vitem.polarity()==VolumeItem::Polarity::REVERSED){
            sx*=-1;
        }


        for( auto i = vbounds.i1(); i<=vbounds.i2(); i++){
            //auto g1d=vitem->volume()->atIT(i,0.001*t);
            auto g1d=v->atIT(i,*times);
            if(!g1d) continue;

            QTransform tf;
            tf.translate(i,0);
            tf.scale(sx,1);
            tf.translate(tx,0);
            if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
                tf*=swappedIlineXlineTransform();
            }

            // wiggles
            auto path=grid1d2wiggles(*g1d);
            auto item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(QPen(Qt::black, 0));
            item->setOpacity(0.01*vitem.opacity());  // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);

            // variable area
            path=grid1d2va(*g1d);
            item=new QGraphicsPathItem();
            item->setPath(path);
            item->setPen(Qt::NoPen);
            item->setBrush(Qt::black);
            item->setOpacity(0.01*vitem.opacity());        // percent -> fraction
            item->setTransform(tf);
            scene->addItem(item);
        }
    }
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
        if(mDisplayOptions.inlineOrientation()==Qt::Vertical){
            xAxis()->setRange(bounds.i1(), bounds.i2());
            xAxis()->setName(tr("Inline"));
            zAxis()->setRange(bounds.j1(), bounds.j2());
            zAxis()->setName(tr("Crossline"));
        }
        else{
            xAxis()->setRange(bounds.j1(), bounds.j2());
            xAxis()->setName(tr("Crossline"));
            zAxis()->setRange(bounds.i1(), bounds.i2());
            zAxis()->setName(tr("Inline"));
        }
        break;
    }
    zoomFitWindow();
}

void VolumeView::updateCompass(){
    switch( m_slice.type){
    case SliceType::Inline:
        mCompassWidget->setMode(CompassWidget::Mode::HORIZONTAL_DIRECTION);
        break;
    case SliceType::Crossline:
        mCompassWidget->setMode(CompassWidget::Mode::HORIZONTAL_DIRECTION);
        break;
    case SliceType::Z:
        mCompassWidget->setMode(CompassWidget::Mode::NEEDLE);
        if(mDisplayOptions.inlineOrientation()==Qt::Vertical){
            // compute angle between inlines and geographic x-axis
            auto il=xAxis()->min();
            auto xl0=zAxis()->min();
            auto xl1=zAxis()->max();
            if(zAxis()->isReversed()){
                std::swap(xl0,xl1);
            }
            QPointF ilxl0(il,xl0);
            QPointF ilxl1(il,xl1);
            QPointF xy0=m_ilxl_to_xy.map(ilxl0);
            QPointF xy1=m_ilxl_to_xy.map(ilxl1);
            auto alpha=std::atan2(xy1.y()-xy0.y(), xy1.x()-xy0.x());
            alpha=180*alpha/M_PI;   // convert to degrees
            if( xAxis()->isReversed()){
                alpha+=180;
            }
            std::cout<<"il="<<il<<" xl0="<<xl0<<" xl1="<<xl1<<" alpha="<<180*alpha/M_PI<<std::endl<<std::flush;
            auto angle=alpha+90;    // make angle refere to y-axis
            mCompassWidget->setAngle(angle);
        }
        else if(mDisplayOptions.inlineOrientation()==Qt::Horizontal){
            // compute angle between crosslines and geographic x-axis
            auto xl=xAxis()->min();
            auto il0=zAxis()->min();
            auto il1=zAxis()->max();
            if(zAxis()->isReversed()){
                std::swap(il0,il1);
            }
            QPointF ilxl0(il0,xl);
            QPointF ilxl1(il1,xl);
            QPointF xy0=m_ilxl_to_xy.map(ilxl0);
            QPointF xy1=m_ilxl_to_xy.map(ilxl1);
            auto alpha=std::atan2(xy1.y()-xy0.y(), xy1.x()-xy0.x());
            alpha=180*alpha/M_PI;   // convert to degrees
            if( xAxis()->isReversed()){
                alpha+=180;
            }
            std::cout<<"il0="<<il0<<" il1="<<il1<<" xl="<<xl<<" alpha="<<180*alpha/M_PI<<std::endl<<std::flush;
            auto angle=alpha+90;    // make angle refere to y-axis
            mCompassWidget->setAngle(angle);
        }
        break;
    }
}

QTransform VolumeView::swappedIlineXlineTransform()const{
    QTransform tf;
    tf.scale(1,-1);
    tf.translate(0,-sceneRect().x());
    tf.rotate(-90);
    tf.translate(-sceneRect().x(),0);
    return tf;
}


void VolumeView::refreshSceneCaller(){
    refreshScene();
}

void VolumeView::onVolumeItemModelChanged(){
    emit volumesChanged();
    updateBounds();
    refreshScene();
}

void VolumeView::updateBounds(){

    if( mVolumeItemModel->size()<1){
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

    for( int i=0; i<mVolumeItemModel->size(); i++){
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;
        auto bounds=vitem->volume()->bounds();

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
    updateCompass();
}


}
