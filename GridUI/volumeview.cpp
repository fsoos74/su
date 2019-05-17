#include "volumeview.h"

#include<QGraphicsPixmapItem>
#include<QImage>
#include<iostream>
#include<cmath>
#include<volumepicker.h>
#include<matrix.h>
#include"volumeitem.h"
#include"horizonitem.h"
#include"wellitem.h"
#include"markeritem.h"
#include"tableitem.h"
#include"viewitemmodel.h"
#include"imageprocessing.h"


namespace sliceviewer {


bool operator==(const VolumeView::SliceDef& a, const VolumeView::SliceDef& b){
    return a.type==b.type && a.value==b.value;
}

VolumeView::VolumeView(QWidget* parent):RulerAxisView(parent), m_flattenRange(0.,0.),
    m_picker(new VolumePicker(this)),
    mHorizonItemModel(new ViewItemModel(this)),
    mVolumeItemModel(new ViewItemModel(this)),
    mWellItemModel(new ViewItemModel(this)),
    mMarkerItemModel(new ViewItemModel(this)),
    mTableItemModel(new ViewItemModel(this))
{
    connect(mVolumeItemModel,SIGNAL(changed()), this, SLOT(onVolumeItemModelChanged()));
    connect(mVolumeItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mHorizonItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mHorizonItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mWellItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mWellItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    connect(mMarkerItemModel,SIGNAL(changed()), this, SLOT(refreshSceneCaller()));
    connect(mMarkerItemModel,SIGNAL(itemChanged(ViewItem*)), this, SLOT(refreshSceneCaller()));
    updateBounds();
    refreshScene();
}

void VolumeView::setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy){
    m_xy_to_ilxl=xy_to_ilxl;
    m_ilxl_to_xy=ilxl_to_xy;
    refreshScene();
}

void VolumeView::setDisplayOptions(const DisplayOptions & opts){
    mDisplayOptions=opts;
    refreshScene();
}

void VolumeView::setInlineOrientation(Qt::Orientation o){
    if(o==mInlineOrientation) return;
    mInlineOrientation=o;
    updateAxes();
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


QVector<QPointF> VolumeView::intersectTableInline(const Table &table, int iline){

    auto points=table.atKey1(iline);
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<1>(p), 1000*std::get<2>(p)));   // millis
    }
    return res;
}

QVector<QPointF> VolumeView::intersectTableCrossline(const Table &table, int xline){
    auto points=table.atKey2(xline);
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<0>(p), 1000*std::get<2>(p)));   // millis
    }
    return res;
}

QVector<QPointF> VolumeView::intersectTableTime(const Table &table, int time){

    auto points=table.inValueRange(0.001*time-0.01,0.001*time+0.01 );           // msec -> sec
    QVector<QPointF> res;
    for( auto p : points ){
        res.push_back(QPointF(std::get<0>(p), std::get<1>(p)));
    }

    return res;
}

QPainterPath VolumeView::projectWellPathInline(const WellPath& wp, int iline){

    QPainterPath path;
    bool isDown=false;

    for( int i=0; i<wp.size(); i++){

        auto p=wp[i];
        auto ilxl=m_xy_to_ilxl.map( QPointF(p.x(), p.y() ) );
        auto xl=ilxl.y(); // static_cast<int>(std::round(ilxl.y()));

        if( std::fabs(ilxl.x()-iline)>mDisplayOptions.wellVisibilityDistance()){
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

        if( std::fabs(ilxl.y()-xline)>mDisplayOptions.wellVisibilityDistance()){
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


void VolumeView::renderVolumes(QGraphicsScene * scene){

    switch(m_slice.type){

    case SliceType::Inline:
        renderVolumesInline(scene);
        break;
    case SliceType::Crossline:
        renderVolumesCrossline(scene);
        break;
    case SliceType::Z:
        renderVolumesTime(scene);
        break;
    }
}

void VolumeView::renderHorizons(QGraphicsScene* scene){

    for( int i=mHorizonItemModel->size()-1; i>=0; i--){

        auto hi=dynamic_cast<HorizonItem*>(mHorizonItemModel->at(i));
        if(!hi) continue;

        QPainterPath path;
        switch(m_slice.type){
        case SliceType::Inline:
            path=intersectHorizonInline(*hi->horizon(), m_slice.value);
            break;
        case SliceType::Crossline:
            path=intersectHorizonCrossline(*hi->horizon(), m_slice.value);
            break;
        default:
            break;
        }

        if( !path.isEmpty()){
            auto item=new QGraphicsPathItem(path);
            QPen pen(hi->color(), hi->width());
            pen.setCosmetic(true);
            item->setPen(pen);
            item->setOpacity(0.01*hi->opacity());   // percent -> fraction
            scene->addItem(item);
        }
    }
}

void VolumeView::renderWells(QGraphicsScene * scene){

    QTransform tf;
    if(m_slice.type==SliceType::Z && mInlineOrientation==Qt::Horizontal){
        tf=swappedIlineXlineTransform();
    }

    for( int i=mWellItemModel->size()-1; i>=0; i-- ){
        auto witem=dynamic_cast<WellItem*>(mWellItemModel->at(i));
        if(!witem) continue;
        auto wp=witem->wellPath();
        if( !wp) continue;

        if(m_slice.type==SliceType::Inline || m_slice.type==SliceType::Crossline){
            QPainterPath path;
            if(m_slice.type==SliceType::Inline){
                path=projectWellPathInline(*wp, m_slice.value);
            }
            else{
                path=projectWellPathCrossline(*wp, m_slice.value);
            }
            if( !path.isEmpty()){
                QPen wellPen( witem->color(), witem->width());
                wellPen.setCosmetic(true);
                auto item=new QGraphicsPathItem(path);
                item->setPen(wellPen);
                scene->addItem(item);
            }
        }
        else if(m_slice.type==SliceType::Z){
            auto p=m_xy_to_ilxl.map( wp->locationAtZ(-m_slice.value) );
            auto s=witem->width();
            auto item=new QGraphicsEllipseItem( -s,-s, 2*s, 2*s);
            item->setPen(Qt::NoPen);
            item->setBrush(witem->color());
            p=tf.map(p);
            item->setPos(p);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            scene->addItem(item);
        }
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

        QPen wmPen(mitem->color(), mitem->width());
        wmPen.setCosmetic(true);
        QFont wmFont("Helvetica [Cronyx]", 8);
        QPen wmLabelPen(mitem->color(),0);
        QGraphicsLineItem* item=new QGraphicsLineItem( -10, 0, 10, 0);
        item->setPen(wmPen);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        item->setPos( x, y);
        scene->addItem(item);
        auto litem=new QGraphicsSimpleTextItem(mitem->name());
        litem->setPen(wmLabelPen);
        litem->setFont(wmFont);
        litem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        litem->setPos(x, y );
        scene->addItem(litem);
    }
}

void VolumeView::renderTables(QGraphicsScene* scene){

    for( int i=mTableItemModel->size()-1; i>=0; i--){
        auto titem=dynamic_cast<TableItem*>(mTableItemModel->at(i));
        if(!titem) continue;
        auto table=titem->table();
        QVector<QPointF> points;
        switch(m_slice.type){
        case SliceType::Inline:
            points=intersectTableInline(*table, m_slice.value);
            break;
        case SliceType::Crossline:
            points=intersectTableCrossline(*table, m_slice.value);
            break;
        case SliceType::Z:
            points=intersectTableTime(*table, m_slice.value);
            break;
        default:
            break;
        }

        auto s=titem->pointSize();
        QBrush brush(titem->color());
        for( auto p : points){
            auto item=new QGraphicsRectItem( -s/2,-s/2, s, s);
            item->setPen(Qt::NoPen);
            item->setBrush(brush);
            item->setPos(p);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            scene->addItem(item);
        }
    }
}


void VolumeView::drawForeground(QPainter *painter, const QRectF &rectInScene){

    painter->save();
    painter->setWorldMatrixEnabled(false);

    QVector<QPointF> points;
    switch(m_slice.type){
    case SliceType::Inline:
        points=intersectTableInline(*m_picker->picks(), m_slice.value);
        break;
    case SliceType::Crossline:
        points=intersectTableCrossline(*m_picker->picks(), m_slice.value);
        break;
    case SliceType::Z:
        points=intersectTableTime(*m_picker->picks(), m_slice.value);
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
        scene->addItem(item);
    }
}

QImage VolumeView::sharpen(QImage src){
    if(!mDisplayOptions.isSharpen()) return src;
    auto kernel=imageprocessing::buildSharpenKernel(mDisplayOptions.sharpenFilterSize(),
                                                    0.01*mDisplayOptions.sharpenFilterStrength());
    return imageprocessing::convolve(src,kernel);
}

namespace{
double lininterp(double x1, double y1, double x2, double y2, double x){

    if( x<=x1 ) return y1;
    if( x>=x2 ) return y2;
    if( x1 == x2 ) return (y1+y2)/2;

    // need to add eps check

    return y1 + (x-x1)* ( y2 - y1 ) / ( x2 - x1 );
}
}

void VolumeView::renderVolumesInline(QGraphicsScene * scene){

    if( !scene ) return;
    int il=m_slice.value;
    auto bounds=m_bounds;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();
        auto dt=v->bounds().dt();
        auto nt=v->bounds().nt();
        auto ft=vbounds.ft()-0.001*m_flattenRange.second;
        auto lt=vbounds.lt()-0.001*m_flattenRange.first;

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto ct = vitem->colorTable();
            QImage vimg=intersectVolumeInline(*v, ct, il, ft, lt);
            vimg=sharpen(vimg);
            QPixmap pixmap=QPixmap::fromImage(vimg);

            auto item=new QGraphicsPixmapItem();
            item->setTransformationMode(Qt::SmoothTransformation);
            item->setPixmap( pixmap );
            item->setOpacity(0.01*vitem->opacity());    // percent > fraction
            scene->addItem(item);

            QTransform tf;
            tf.translate(xAxis()->min(), zAxis()->min());
            tf.scale((xAxis()->max()-xAxis()->min())/vimg.width(), (zAxis()->max()-zAxis()->min())/vimg.height());
            item->setTransform(tf);
        }
        else{                                           // render as seismic traces
            auto ct = vitem->colorTable();  // use this to determine max abs value
            auto m1=std::abs(ct->range().first);
            auto m2=std::abs(ct->range().second);
            auto maxabs=(m1>m2) ? m1 : m2;

            for( auto i = 0; i<bounds.nj(); i++){
                QPainterPath path;
                bool first=true;
                auto xl=bounds.j1()+i;
                auto d=dz(il, xl);
                if( std::isnan(d)) continue;

                // wiggles
                for( auto j=0; j<nt; j++){
                    auto z=ft + j*dt + d;
                    auto value=v->value(il, xl, z);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = xl + value/maxabs;
                    z*=1000;
                    if(first){
                        path.moveTo(x,z);
                        first=false;
                    }
                    else{
                        path.lineTo(x,z);
                    }
                }
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());  // percent -> fraction
                scene->addItem(item);

                // variable area
                path=QPainterPath();
                bool active=false;
                double xprev=xl;
                double zprev=ft+d;
                for( auto j=0; j<nt; j++){
                    auto z=ft + j*dt + d;
                    auto value=v->value(il, xl, z);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = xl + value/maxabs;
                    z*=1000;
                    if(x>=xl){               // inside va
                        if(!active){        // no area yet, start new
                            auto zxl=(j>0) ? lininterp(xprev, zprev,x,z,xl) : z;
                            path.moveTo(xl,zxl);
                            //path.moveTo(x,z);
                            active=true;
                        }
                        path.lineTo(x,z);   // line to current point
                    }
                    else if(active){        // close area
                        auto zxl=lininterp(x,z,xprev, zprev,xl);
                        path.lineTo(xl,zxl);
                        //path.lineTo(x,z);
                        active=false;
                        path.closeSubpath();
                    }
                    xprev=x;
                    zprev=z;
                }
                //path.closeSubpath();
                item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(Qt::NoPen);
                item->setBrush(Qt::black);
                item->setOpacity(0.01*vitem->opacity());        // percent -> fraction
                scene->addItem(item);

            }
        }
    }

    //scene->setSceneRect(xAxis()->min(), zAxis()->min(), xAxis()->max() - xAxis()->min(), zAxis()->max()-zAxis()->min());
}


void VolumeView::renderVolumesCrossline(QGraphicsScene * scene){

    if( !scene ) return;
    int xl=m_slice.value;
    auto bounds=m_bounds;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();
        auto dt=v->bounds().dt();
        auto nt=v->bounds().nt();
        auto ft=vbounds.ft()-0.001*m_flattenRange.second;
        auto lt=vbounds.lt()-0.001*m_flattenRange.first;

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto ct = vitem->colorTable();
            QImage vimg=intersectVolumeCrossline(*v, ct, xl, ft, lt);
            vimg=sharpen(vimg);
            QPixmap pixmap=QPixmap::fromImage(vimg);

            auto item=new QGraphicsPixmapItem();
            item->setTransformationMode(Qt::SmoothTransformation);
            item->setPixmap( pixmap );
            item->setOpacity(0.01*vitem->opacity());        // percent -> fraction
            scene->addItem(item);

            QTransform tf;
            tf.translate(xAxis()->min(), zAxis()->min());
            tf.scale((xAxis()->max()-xAxis()->min())/vimg.width(), (zAxis()->max()-zAxis()->min())/vimg.height());
            item->setTransform(tf);
        }
        else{                                           // render as seismic traces
            auto ct = vitem->colorTable();  // use this to determine max abs value
            auto m1=std::abs(ct->range().first);
            auto m2=std::abs(ct->range().second);
            auto maxabs=(m1>m2) ? m1 : m2;

            for( auto i = 0; i<bounds.ni(); i++){
                QPainterPath path;
                bool first=true;
                auto il=bounds.i1()+i;
                auto d=dz(il, xl);
                if( std::isnan(d)) continue;

                // wiggles
                for( auto j=0; j<nt; j++){
                    auto z=ft + j*dt + d;
                    auto value=v->value(il, xl, z);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = il + value/maxabs;
                    z*=1000;
                    if(first){
                        path.moveTo(x,z);
                        first=false;
                    }
                    else{
                        path.lineTo(x,z);
                    }
                }
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());// percent -> fraction
                scene->addItem(item);

                // variable area
                path=QPainterPath();
                bool active=false;
                double xprev=xl;
                double zprev=ft+d;
                for( auto j=0; j<nt; j++){
                    auto z=ft + j*dt + d;
                    auto value=v->value(il, xl, z);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = il + value/maxabs;
                    z*=1000;
                    if(x>=il){               // inside va
                        if(!active){        // no area yet, start new
                            auto zil=(j>0) ? lininterp(xprev, zprev,x,z,il) : z;
                            path.moveTo(il,zil);
                            //path.moveTo(x,z);
                            active=true;
                        }
                        path.lineTo(x,z);   // line to current point
                    }
                    else if(active){        // close area
                        auto zil=lininterp(x,z,xprev, zprev,il);
                        path.lineTo(il,zil);
                        //path.lineTo(x,z);
                        active=false;
                        path.closeSubpath();
                    }
                    xprev=x;
                    zprev=z;
                }
                //path.closeSubpath();
                item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(Qt::NoPen);
                item->setBrush(Qt::black);
                item->setOpacity(0.01*vitem->opacity());        // percent -> fraction
                scene->addItem(item);

            }
        }
    }

    //scene->setSceneRect(xAxis()->min(), zAxis()->min(), xAxis()->max() - xAxis()->min(), zAxis()->max()-zAxis()->min());
}


void VolumeView::renderVolumesTime(QGraphicsScene * scene){

    if( !scene ) return;
    int t=m_slice.value;
    auto bounds=m_bounds;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto ct = vitem->colorTable();
            QImage vimg=intersectVolumeTime(*v, ct, t);
            vimg=sharpen(vimg);
            QPixmap pixmap=QPixmap::fromImage(vimg);

            auto item=new QGraphicsPixmapItem();
            item->setTransformationMode(Qt::SmoothTransformation);
            item->setPixmap( pixmap );
            item->setOpacity(0.01*vitem->opacity());    // percent > fraction
            scene->addItem(item);

            QTransform tf;
            if(mInlineOrientation==Qt::Vertical){
                tf.translate(xAxis()->min(), zAxis()->min());
                tf.scale((xAxis()->max()-xAxis()->min())/vimg.width(), (zAxis()->max()-zAxis()->min())/vimg.height());
            }
            else{
                tf.translate(xAxis()->min(), zAxis()->min());
                tf.scale((zAxis()->max()-zAxis()->min())/vimg.width(), -(xAxis()->max()-xAxis()->min())/vimg.height());
                tf.rotate(-90);
            }
            item->setTransform(tf);
        }
        else{                                           // render as seismic traces
            auto ct = vitem->colorTable();  // use this to determine max abs value
            auto m1=std::abs(ct->range().first);
            auto m2=std::abs(ct->range().second);
            auto maxabs=(m1>m2) ? m1 : m2;

            QTransform tf;
            if(mInlineOrientation==Qt::Horizontal){
                tf=swappedIlineXlineTransform();
            }

            for( auto i = 0; i<bounds.ni(); i++){
                QPainterPath path;
                bool first=true;
                auto il=bounds.i1()+i;

                // wiggles
                for( auto j=0; j<bounds.nj(); j++){
                    int xl=bounds.j1()+j;
                    auto d=dz(il, xl);
                    if( std::isnan(d)) continue;
                    auto tt=0.001*t;// + d;
                    auto value=v->value(il, xl, tt);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = il + value/maxabs;
                    if(first){
                        path.moveTo(x,xl);
                        first=false;
                    }
                    else{
                        path.lineTo(x,xl);
                    }
                }
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());  // percent -> fraction
                item->setTransform(tf);
                scene->addItem(item);

                // variable area
                path=QPainterPath();
                bool active=false;
                double xprev=il;
                double zprev=bounds.j1();
                for( auto j=0; j<bounds.nj(); j++){
                    int xl=bounds.j1()+j;
                    auto d=dz(il, xl);
                    if( std::isnan(d)) continue;
                    auto tt=0.001*t;// + d;
                    auto value=v->value(il, xl, tt);
                    if( value==v->NULL_VALUE) continue;
                    qreal x = il + value/maxabs;
                    qreal z=xl;
                    if(x>=il){               // inside va
                        if(!active){        // no area yet, start new
                            auto zil=(j>0) ? lininterp(xprev, zprev,x,z,il) : z;
                            path.moveTo(x,zil);
                            active=true;
                        }
                        path.lineTo(x,z);   // line to current point
                    }
                    else if(active){        // close area
                        auto zil=lininterp(x,z, xprev,zprev, il);
                        path.lineTo(x,zil);
                        active=false;
                        path.closeSubpath();
                    }
                    xprev=x;
                    zprev=z;
                }
                //path.closeSubpath();
                item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(Qt::NoPen);
                item->setBrush(Qt::black);
                item->setOpacity(0.01*vitem->opacity());        // percent -> fraction
                item->setTransform(tf);
                scene->addItem(item);
            }
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
        if(mInlineOrientation==Qt::Vertical){
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

QTransform VolumeView::swappedIlineXlineTransform()const{
    QTransform tf;
    tf.translate(0,sceneRect().y());
    tf.scale(1,-1);
    tf.translate(0,-sceneRect().y());
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
}


}
