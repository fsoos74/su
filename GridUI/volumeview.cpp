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

        std::cout<<"i="<<i<<" value="<<value<<std::endl<<std::flush;
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
    setRenderHint(QPainter::RenderHint::Antialiasing, mDisplayOptions.isAntiliasing());
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

    if(m_slice.type==SliceType::Z) return;

    for( int i=mHorizonItemModel->size()-1; i>=0; i--){

        auto hi=dynamic_cast<HorizonItem*>(mHorizonItemModel->at(i));
        if(!hi) continue;
        auto g1d=(m_slice.type==SliceType::Inline) ? hi->horizon()->atI(m_slice.value)
                                                   : hi->horizon()->atJ(m_slice.value);
        if(!g1d) continue;

        QPen pen(hi->color(), hi->width());
        pen.setCosmetic(true);
        auto path=grid1d2hpath(*g1d);
        auto item=new QGraphicsPathItem(path);
        item->setPen(pen);
        item->setOpacity(0.01*hi->opacity());   // percent -> fraction
        scene->addItem(item);
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
        if(m_slice.type==SliceType::Z && mInlineOrientation==Qt::Horizontal){
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

void VolumeView::renderVolumesInline(QGraphicsScene * scene){

    if( !scene ) return;
    int il=m_slice.value;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();
        auto ct = vitem->colorTable();

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto g2d=v->atI(il);
            if(!g2d){
                continue;
            }
            // apply gain, polarity
            double b=0;
            double m=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
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
            item->setOpacity(0.01*vitem->opacity());    // percent > fraction

            QTransform tf;
            tf.translate(vbounds.j1(),vbounds.ft());
            tf.scale(1,1000.*vbounds.dt());         // sec -> msec
            item->setTransform(tf);
            scene->addItem(item);
        }
        else{
            if(!mVolumeStatistics.contains(vitem->name())){
                mVolumeStatistics.insert(vitem->name(),
                    statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
            }
            auto stats=mVolumeStatistics.value(vitem->name());
            auto tx=-stats.rms;
            auto sx=1./(3*stats.sigma);  // 3 standard deviations
            sx*=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
                sx*=-1;
            }
std::cout<<"rms="<<stats.rms<<" maxabs="<<stats.maxabs<<" sx="<<sx<<" tx="<<tx<<std::endl<<std::flush;
            for( auto j = vbounds.j1(); j<=vbounds.j2(); j++){
                auto g1d=vitem->volume()->atIJ(il,j);
                if(!g1d) continue;

                QTransform tf;
                tf.translate(j,vbounds.ft());
                tf.scale(sx,1000*vbounds.dt());
                tf.translate(tx,0);

                // wiggles
                auto path=grid1d2wiggles(*g1d);
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());  // percent -> fraction
                item->setTransform(tf);
                scene->addItem(item);

                // variable area
                path=grid1d2va(*g1d);
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


void VolumeView::renderVolumesCrossline(QGraphicsScene * scene){

    if( !scene ) return;
    int xl=m_slice.value;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();
        auto ct = vitem->colorTable();

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto g2d=v->atJ(xl);
            if(!g2d){
                continue;
            }
            // apply gain, polarity
            double b=0;
            double m=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
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
            item->setOpacity(0.01*vitem->opacity());    // percent > fraction

            QTransform tf;
            tf.translate(vbounds.i1(),vbounds.ft());
            tf.scale(1,1000.*vbounds.dt());         // sec -> msec
            item->setTransform(tf);
            scene->addItem(item);
        }
        else{
            // render as seismic traces
            if(!mVolumeStatistics.contains(vitem->name())){
                mVolumeStatistics.insert(vitem->name(),
                    statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
            }
            auto stats=mVolumeStatistics.value(vitem->name());
            auto tx=-stats.rms;
            auto sx=1./(stats.maxabs-stats.rms);//std::fabs(mean));
            sx*=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
                sx*=-1;
            }

            for( auto i = vbounds.i1(); i<=vbounds.i2(); i++){
                auto g1d=vitem->volume()->atIJ(i,xl);
                if(!g1d) continue;

                QTransform tf;
                tf.translate(i,vbounds.ft());
                tf.scale(sx,1000*vbounds.dt());
                tf.translate(tx,0);

                // wiggles
                auto path=grid1d2wiggles(*g1d);
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());  // percent -> fraction
                item->setTransform(tf);
                scene->addItem(item);

                // variable area
                path=grid1d2va(*g1d);
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


void VolumeView::renderVolumesTime(QGraphicsScene * scene){

    if( !scene ) return;
    int t=m_slice.value;
    auto bounds=m_bounds;

    for( int i=mVolumeItemModel->size()-1; i>=0; i--){      // iterate in reverse order, first item in list is on top
        auto vitem=dynamic_cast<VolumeItem*>(mVolumeItemModel->at(i));
        if(!vitem) continue;

        auto v=vitem->volume();
        auto vbounds=v->bounds();
        auto ct = vitem->colorTable();

        if(vitem->style()==VolumeItem::Style::ATTRIBUTE){   // render as image
            auto g2d=vitem->volume()->atT(0.001*t);          // msec -> sec
            if(!g2d) continue;
            // apply gain, polarity
            double b=0;
            double m=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
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
            item->setOpacity(0.01*vitem->opacity());    // percent > fraction

            QTransform tf;
            tf.translate(v->bounds().i1(), v->bounds().j1());
            tf.scale(qreal(bounds.i2()-bounds.i1())/vimg.width(), qreal(bounds.j2()-bounds.j1())/vimg.height());
            if(mInlineOrientation==Qt::Horizontal){
                tf*=swappedIlineXlineTransform();
            }
            item->setTransform(tf);
            scene->addItem(item);
        }
        else{
            // render as seismic traces
            if(!mVolumeStatistics.contains(vitem->name())){
                mVolumeStatistics.insert(vitem->name(),
                    statistics::computeStatistics(v->cbegin(), v->cend(), v->NULL_VALUE) );
            }
            auto stats=mVolumeStatistics.value(vitem->name());
            auto tx=-stats.rms;
            auto sx=1./(stats.maxabs-stats.rms);//std::fabs(mean));
            sx*=vitem->gain();
            if(vitem->polarity()==VolumeItem::Polarity::REVERSED){
                sx*=-1;
            }


            for( auto i = vbounds.i1(); i<=vbounds.i2(); i++){
                auto g1d=vitem->volume()->atIT(i,0.001*t);
                if(!g1d) continue;

                QTransform tf;
                tf.translate(i,0);
                tf.scale(sx,1);
                tf.translate(tx,0);
                if(mInlineOrientation==Qt::Horizontal){
                    tf*=swappedIlineXlineTransform();
                }

                // wiggles
                auto path=grid1d2wiggles(*g1d);
                auto item=new QGraphicsPathItem();
                item->setPath(path);
                item->setPen(QPen(Qt::black, 0));
                item->setOpacity(0.01*vitem->opacity());  // percent -> fraction
                item->setTransform(tf);
                scene->addItem(item);

                // variable area
                path=grid1d2va(*g1d);
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
