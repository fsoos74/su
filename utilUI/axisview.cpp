#include "axisview.h"

#include<QScrollBar>
#include<QGraphicsPathItem>
#include<QMouseEvent>
#include<QKeyEvent>
#include<cmath>
#include<iostream>


template<class R> void dumpRect(R rect){
    std::cout<<"x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl<<std::flush;
}

AxisView::AxisView(QWidget* parent):QGraphicsView(parent),
       m_xAxis(new Axis(Axis::Type::Linear, 0, 1, this) ),
       m_zAxis(new Axis(Axis::Type::Linear, 0, 1, this) )
{

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_selectionRubberband=new QRubberBand(QRubberBand::Shape::Rectangle, this);
    m_selectionRubberband->hide();

    m_xCursorRubberband=new QRubberBand(QRubberBand::Shape::Line, this );
    m_xCursorRubberband->hide();

    //m_zCursorRubberband=new QRubberBand(QRubberBand::Shape::Line, this );
    //m_zCursorRubberband->hide();

    connect(m_xAxis, SIGNAL(ticksChanged()), this, SLOT(ticksChanged()) );//  update()));//SLOT(xTicksChanged()) );
    connect(m_xAxis, SIGNAL(reversedChanged(bool)), this, SLOT(revertXAxis()));
    connect(m_xAxis, SIGNAL(typeChanged(Type)), this, SLOT(axisTypesChanged()));//updateSceneRectFromAxes() ) );
    connect(m_xAxis, SIGNAL(rangeChanged(qreal,qreal)), this, SLOT(updateSceneRectFromAxes() ) );
    connect(m_xAxis, SIGNAL(selectionChanged(qreal,qreal)), this, SLOT(showXSelection(qreal,qreal)) );
    connect(m_xAxis, SIGNAL(cursorPositionChanged(qreal)), this, SLOT(showXCursor(qreal)) );
    connect(m_xAxis, SIGNAL(viewRangeChanged(qreal,qreal)), this, SLOT(updateViewRectFromAxes()) );
    connect(m_xAxis, SIGNAL(nameChanged(QString)), this, SLOT(updateBackground()) );
    connect(m_xAxis, SIGNAL(unitChanged(QString)), this, SLOT(updateBackground()) );
    connect(m_xAxis, SIGNAL(labelPrecisionChanged(int)), this, SLOT(updateBackground()));

    connect(m_zAxis, SIGNAL(ticksChanged()), this, SLOT(ticksChanged()));// update()));//SLOT(zTicksChanged()) );
    connect(m_zAxis, SIGNAL(reversedChanged(bool)), this, SLOT(revertZAxis()));
    connect(m_zAxis, SIGNAL(typeChanged(Type)), this, SLOT(axisTypesChanged()));//updateSceneRectFromAxes() ) );
    connect(m_zAxis, SIGNAL(rangeChanged(qreal,qreal)), this, SLOT(updateSceneRectFromAxes() ) );
    connect(m_zAxis, SIGNAL(selectionChanged(qreal,qreal)), this, SLOT(showZSelection(qreal,qreal)) );
    connect(m_zAxis, SIGNAL(cursorPositionChanged(qreal)), this, SLOT(showZCursor(qreal)) );
    connect(m_zAxis, SIGNAL(viewRangeChanged(qreal,qreal)), this, SLOT(updateViewRectFromAxes()) );
    connect(m_zAxis, SIGNAL(nameChanged(QString)), this, SLOT(updateBackground()) );
    connect(m_zAxis, SIGNAL(unitChanged(QString)), this, SLOT(updateBackground()) );
    connect(m_zAxis, SIGNAL(labelPrecisionChanged(int)), this, SLOT(updateBackground()));

    //setCacheMode(QGraphicsView::CacheBackground);   // IMPORTANT: NEED THIS TO PROPERLY RESTORE MESH WHEN USING RUBBERBAND
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(true); // continuosly emit mouseOver

    setSceneRect(sceneRectFromAxes());      // sync from start
    fitInView(sceneRect(),Qt::IgnoreAspectRatio);   // this is done in resize event
}


QRectF AxisView::sceneRectFromAxes(){
    if( !m_xAxis || !m_zAxis) return QRectF();
    auto x1 = m_xAxis->toScene(m_xAxis->min());
    auto x2 = m_xAxis->toScene(m_xAxis->max());
    auto z1 = m_zAxis->toScene(m_zAxis->min());
    auto z2 = m_zAxis->toScene(m_zAxis->max());
    return QRectF( x1, z1, x2-x1, z2-z1).normalized();
}

QRectF AxisView::viewRectFromAxes(){        // same unit as scenerect, scene coords
    auto x1 = m_xAxis->toScene(m_xAxis->minView());
    auto x2 = m_xAxis->toScene(m_xAxis->maxView());
    auto z1 = m_zAxis->toScene(m_zAxis->minView());
    auto z2 = m_zAxis->toScene(m_zAxis->maxView());
    return QRectF( x1, z1, x2-x1, z2-z1).normalized();
}

QRectF AxisView::sceneRectFromView(){
    return sceneRect();
}

QRectF AxisView::viewRectFromView(){        // in scene coords
    auto vr=viewport()->rect();
    return mapToScene(vr).boundingRect();
}

void AxisView::clearSelection(){
    m_selectedPoints.clear();
    invalidateScene(sceneRect(),QGraphicsScene::ForegroundLayer);
    scene()->clearSelection();
    scene()->update();
}

void AxisView::setKeepAspectRatio(bool on){
    if(on==m_keepAspectRatio) return;
    m_keepAspectRatio=on;
    emit keepAspectRatioChanged(on);
}

void AxisView::setZoomMode(ZOOMMODE m){

    if( m==m_zoomMode) return;
    m_zoomMode=m;
}

void AxisView::setCursorMode(CURSORMODE m){

    if( m==m_cursorMode) return;

    m_cursorMode=m;

    switch( m_cursorMode ){
    case CURSORMODE::NONE:
        showXCursor(Axis::NO_CURSOR);
        showZCursor(Axis::NO_CURSOR);
        break;
    case CURSORMODE::X:
        showZCursor(Axis::NO_CURSOR);
        break;
    case CURSORMODE::Z:
        showXCursor(Axis::NO_CURSOR);
        break;
    default:
        break;
    }
}

void AxisView::setMouseMode(MouseMode m){

    if( m==m_mouseMode) return;
    m_mouseMode=m;
    emit mouseModeChanged(m);
}

void AxisView::setXAxis(Axis * axis){

    if( m_xAxis ) delete m_xAxis;
    setXAxisNoDelete(axis);
}


void AxisView::setZAxis(Axis * axis){

    if( m_zAxis ) delete m_zAxis;
    setZAxisNoDelete(axis);
}

void AxisView::setXAxisNoDelete(Axis * axis){
    m_xAxis=axis;
    if(axis){
        connect(axis, SIGNAL(ticksChanged()), this, SLOT(ticksChanged()));//xTicksChanged()) );
        connect(axis, SIGNAL(reversedChanged(bool)), this, SLOT(revertXAxis()));
        connect(axis, SIGNAL(typeChanged(Type)), this, SLOT(updateSceneRectFromAxes() ) );
        connect(axis, SIGNAL(rangeChanged(qreal,qreal)), this, SLOT(updateSceneRectFromAxes() ) );
        connect(axis, SIGNAL(selectionChanged(qreal,qreal)), this, SLOT(showXSelection(qreal,qreal)) );
        connect(axis, SIGNAL(cursorPositionChanged(qreal)), this, SLOT(showXCursor(qreal)) );
        connect(axis, SIGNAL(viewRangeChanged(qreal,qreal)), this, SLOT(updateViewRectFromAxes()) );
        connect(axis, SIGNAL(nameChanged(QString)), this, SLOT(updateBackground()) );
        connect(axis, SIGNAL(unitChanged(QString)), this, SLOT(updateBackground()) );
        connect(axis, SIGNAL(labelPrecisionChanged(int)), this, SLOT(updateBackground()));

        updateSceneRectFromAxes();
        updateViewRectFromAxes();
        showXSelection(axis->minSelection(), axis->maxSelection()); // XXX
        //axis->setViewPixelLength(viewport()->width());
        refreshScene();
    }
    update();
}

void AxisView::setZAxisNoDelete(Axis * axis){
    m_zAxis=axis;
    if(axis){
        connect(axis, SIGNAL(ticksChanged()), this, SLOT(ticksChanged()));//update()));//zTicksChanged()) );
        connect(axis, SIGNAL(reversedChanged(bool)), this, SLOT(revertZAxis()));
        connect(axis, SIGNAL(typeChanged(Type)), this, SLOT(updateSceneRectFromAxes() ) );
        connect(axis, SIGNAL(rangeChanged(qreal,qreal)), this, SLOT(updateSceneRectFromAxes() ) );
        connect( axis, SIGNAL(selectionChanged(qreal,qreal)), this, SLOT(showZSelection(qreal,qreal)) );
        connect(axis, SIGNAL(cursorPositionChanged(qreal)), this, SLOT(showZCursor(qreal)) );
        connect(axis, SIGNAL(viewRangeChanged(qreal,qreal)), this, SLOT(updateViewRectFromAxes()) );
        connect(axis, SIGNAL(nameChanged(QString)), this, SLOT(updateBackground()) );
        connect(axis, SIGNAL(unitChanged(QString)), this, SLOT(updateBackground()) );
        connect(axis, SIGNAL(labelPrecisionChanged(int)), this, SLOT(updateBackground()));

        updateSceneRectFromAxes();
        updateViewRectFromAxes();
        showZSelection(axis->minSelection(), axis->maxSelection()); // XXX
        //axis->setViewPixelLength(viewport()->height());
        refreshScene();
    }
    update();
}



void AxisView::setXMesh(bool on){
    if( on==m_xMesh) return;
    m_xMesh=on;
    update();
}

void AxisView::setZMesh(bool on){
    if( on==m_zMesh) return;
    m_zMesh=on;
    update();
}


void AxisView::setSelectionMode(SELECTIONMODE  mode){

    if( mode==m_selectionMode) return;

    m_selectionMode=mode;

    // clear all old selections
    //m_selectedPoint=QPointF();
    //m_selectedLine=QLineF();
    //m_selectionPolygon.clear();
    m_selectedPoints.clear();
    if( scene()) scene()->clearSelection();

    // redraw to remove traces of old selections
    invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
    //scene()->update();
    update();
}

void AxisView::setSelectedLine(QLineF l){

    if( l==selectedLine()) return;

    m_selectedPoints.clear();
    m_selectedPoints.push_back(l.p1());
    m_selectedPoints.push_back(l.p2());

    scene()->update();
}

void AxisView::setSelectionPolygon( QPolygonF poly){

    if( poly==selectionPolygon()) return;

    m_selectedPoints=poly.toList().toVector();

    scene()->update();
  //  update();
}


void AxisView::ticksChanged(){
    //refreshScene();   // tics not handled by scene items, instead drawbackground
    updateBackground();
}


void AxisView::revertXAxis(){
    scale( -1, 1);
}

void AxisView::revertZAxis(){
    scale( 1, -1);
}

void AxisView::axisTypesChanged(){  // this also changes the position of items within scene
    updateSceneRectFromAxes();
    refreshScene();         // MUST BE HERE
}

void AxisView::updateSceneRectFromAxes(){

    auto r=sceneRectFromAxes();
    setSceneRect(r);
    updateViewRectFromAxes();
}

void AxisView::updateViewRectFromAxes(){

    auto r=viewRectFromAxes();
    fitInView(r, Qt::IgnoreAspectRatio);       // viewrectfromaxes in scene coords
}

void AxisView::updateAxesRangeFromSceneRect(){
    auto r=sceneRect();
    m_xAxis->setRange( m_xAxis->toAxis(r.left()), m_xAxis->toAxis(r.right()));
    m_zAxis->setRange( m_zAxis->toAxis(r.top()), m_zAxis->toAxis(r.bottom()));
}

void AxisView::updateAxesViewRangeFromViewRect(){
    auto r=viewRectFromView();
    m_xAxis->setViewRange( m_xAxis->toAxis(r.left()), m_xAxis->toAxis(r.right()));
    m_zAxis->setViewRange( m_zAxis->toAxis(r.top()), m_zAxis->toAxis(r.bottom()));
}

void AxisView::showXSelection(qreal start, qreal stop){ // start,stop in axis units

    if( start==stop ){
        m_selectionRubberband->hide();
        return;
    }

    if( start>stop) std::swap(start, stop);

    start=xAxis()->toScene(start);
    stop=xAxis()->toScene(stop);

    if( m_zoomMode==ZOOMMODE::X ){
        auto selectedRect = mapFromScene( QRectF(start, sceneRect().y(), stop-start, sceneRect().height() )).boundingRect();    // viewport coords
        m_selectionRubberband->setGeometry(viewport()->x() + selectedRect.x(), viewport()->y(), selectedRect.width(), viewport()->height());
    }
    else if(m_zoomMode==ZOOMMODE::BOTH){
        QPoint p1=mapFromScene(xAxis()->toScene(xAxis()->minSelection()), zAxis()->toScene(zAxis()->minSelection()));           // viewport coords
        QPoint p2=mapFromScene(xAxis()->toScene(xAxis()->maxSelection()),zAxis()->toScene(zAxis()->maxSelection()));           // viewport coords
        p1+=viewport()->pos();
        p2+=viewport()->pos();
        if( p1.y()!=p2.y()){
           m_selectionRubberband->setGeometry( QRect(p1, p2).normalized());
        }
        else{
           m_selectionRubberband->setGeometry( p1.x(), viewport()->y(), p2.x()-p1.x(), viewport()->height());
        }
    }

    m_selectionRubberband->show();
}

void AxisView::showZSelection(qreal start, qreal stop){ // start.stop are axis values

    if( start==stop ){
        m_selectionRubberband->hide();
        return;
    }

    if( start>stop) std::swap(start, stop);

    start=zAxis()->toScene(start);
    stop=zAxis()->toScene(stop);

    if( m_zoomMode==ZOOMMODE::Z){
        auto selectedRect = mapFromScene( QRectF(sceneRect().x(), start, sceneRect().width(), stop-start)).boundingRect();  // viewport coords
        m_selectionRubberband->setGeometry(viewport()->x(), viewport()->y() + selectedRect.y(), viewport()->width(), selectedRect.height());
    }
    else if(m_zoomMode==ZOOMMODE::BOTH){
        QPoint p1=mapFromScene(xAxis()->toScene(xAxis()->minSelection()),zAxis()->toScene(zAxis()->minSelection()));   // viewport coords
        QPoint p2=mapFromScene(xAxis()->toScene(xAxis()->maxSelection()),zAxis()->toScene(zAxis()->maxSelection()));   // viewport coords
        p1+=viewport()->pos();
        p2+=viewport()->pos();
        if( p1.x()!=p2.x()){
           m_selectionRubberband->setGeometry( QRect(p1, p2).normalized());
        }
        else{
           m_selectionRubberband->setGeometry( viewport()->x(), p1.y(), viewport()->width(), p2.y()-p1.y() );
        }
    }

    m_selectionRubberband->show();
}


void AxisView::showXCursor(qreal x){    // axis coords

    if( x==Axis::NO_CURSOR ){
        if( m_xCursorRubberband ){
            m_xCursorRubberband->hide();
        }
        return;
    }

    if( m_cursorMode!=CURSORMODE::X && m_cursorMode!=CURSORMODE::BOTH) return;

    if(!m_xCursorRubberband) m_xCursorRubberband=new QRubberBand(QRubberBand::Line, this);
    x=xAxis()->toScene(x);
    auto p = mapFromScene( x, sceneRect().top() );      // viewport coords
    m_xCursorRubberband->setGeometry( viewport()->x() + p.x(), viewport()->y(), 1, viewport()->height() );
    m_xCursorRubberband->show();
}

void AxisView::showZCursor(qreal z){    // z is value

    if( z==Axis::NO_CURSOR ){
        if( m_zCursorRubberband ){
            m_zCursorRubberband->hide();
        }
        return;
    }

    if( m_cursorMode!=CURSORMODE::Z && m_cursorMode!=CURSORMODE::BOTH) return;

    if(!m_zCursorRubberband) m_zCursorRubberband=new QRubberBand(QRubberBand::Shape::Line, this );
    z=zAxis()->toScene(z);
    auto p = mapFromScene( sceneRect().left(), z );     // viewport coords
    m_zCursorRubberband->setGeometry( viewport()->x(), viewport()->y() + p.y(), viewport()->width(), 1 );
    m_zCursorRubberband->show();
}


//adjust to aspect ratio by shrinking (new testing)
// NEEDS TO HANDLE LOG, CLIP AT 0!!!, NO NEGATIVE VALUES ALLOWED!!!
QRectF AxisView::adjustViewRect(const QRectF& r){
    if( !m_keepAspectRatio) return r;
    qreal x0=r.x();
    qreal x1=x0+r.width();
    qreal dx=x1-x0;
    qreal z0=r.y();
    qreal z1=z0+r.height();
    qreal dz=z1-z0;
    qreal dxpix=viewport()->width();
    qreal dzpix=viewport()->height();
    qreal ppx=dxpix/dx;
    qreal ppz=dzpix/dz;

    if(ppx>ppz){
        x1=x0+dxpix/ppz;
    }
    else if(ppx<ppz){
        z1=z0+dzpix/ppx;
    }

    dx=x1-x0;
    dz=z1-z0;
    ppx=dxpix/dx;
    ppz=dzpix/dz;

    return QRectF(x0,z0,x1-x0,z1-z0);
}


/*
//adjust to aspect ratio by growing (old working)
QRectF AxisView::adjustViewRect(const QRectF& r){
    if( !m_keepAspectRatio) return r;
    qreal x0=r.x();
    qreal x1=x0+r.width();
    qreal dx=x1-x0;
    qreal z0=r.y();
    qreal z1=z0+r.height();
    qreal dz=z1-z0;
    qreal dxpix=viewport()->width();
    qreal dzpix=viewport()->height();
    qreal ppx=dxpix/dx;
    qreal ppz=dzpix/dz;
    if(ppx>ppz){
        z1=z0+dzpix/ppx;
    }
    else if(ppx<ppz){
        x1=x0+dxpix/ppz;
    }

    dx=x1-x0;
    dz=z1-z0;
    ppx=dxpix/dx;
    ppz=dzpix/dz;

    return QRectF(x0,z0,x1-x0,z1-z0);
}
*/


void AxisView::zoomFitWindow(){

    zoom( sceneRectFromAxes() );
}

void AxisView::zoomBy(qreal factor){
    auto xc = ( m_xAxis->minView() + m_xAxis->maxView() ) / 2;
    auto w= m_xAxis->maxView() - m_xAxis->minView();
    w*=factor;
    m_xAxis->setViewRange( std::max( m_xAxis->min(), xc - w/2), std::min(m_xAxis->max(), xc+w/2) );

    auto zc = ( m_zAxis->minView() + m_zAxis->maxView() ) / 2;
    auto h= m_zAxis->maxView() - m_zAxis->minView();
    h*=factor;
    m_zAxis->setViewRange( std::max( m_zAxis->min(), zc - h/2), std::min(m_zAxis->max(), zc+h/2) );
}

void AxisView::zoom(QRectF rect){
    QRectF r=adjustViewRect( rect );
    qreal x0=r.x();
    qreal x1=x0+r.width();
    qreal z0=r.y();
    qreal z1=z0+r.height();

    m_xAxis->setViewRange( x0, x1);
    m_zAxis->setViewRange( z0, z1 );
}

void AxisView::zoomIn(){
    zoomBy(0.8);
}

void AxisView::zoomOut(){
    zoomBy(1.25);
}

void AxisView::mousePressEvent(QMouseEvent * event){
    QPointF scenePos=mapToScene(event->pos());
    QPointF p=toAxis(scenePos);// axisPos( xAxis()->toAxis(scenePos.x()), zAxis()->toAxis(scenePos.y()));

    // begin zoom on ctrl + left button
    if( event->button() == Qt::LeftButton && (event->modifiers() == Qt::ControlModifier || m_mouseMode==MouseMode::Zoom) ){

        auto p = mapToScene(event->pos() );//- viewport()->pos() ); // YYY
        p.setX(m_xAxis->toAxis(p.x()));     // store axis coords
        p.setY(m_zAxis->toAxis(p.y()));     // store axis coords

        switch ( m_zoomMode ) {
        case ZOOMMODE::X:
            m_selStart=p;
            m_zoomActive=true;
            break;
        case ZOOMMODE::Z:
            m_selStart=p;
            m_zoomActive=true;
            break;
        case ZOOMMODE::BOTH:
            m_selStart=p;
            m_zoomActive=true;
            break;
        default:
            break;
        }
    }
    else if( event->button()==Qt::LeftButton &&
             ( event->modifiers()==Qt::ShiftModifier ||  m_mouseMode==MouseMode::Select || m_mouseMode==MouseMode::Pick  ) ){

        m_selectionActive=true;

        switch(m_selectionMode){
        case SELECTIONMODE::SinglePoint:
            m_selectedPoints=QVector<QPointF>{p};
            emit pointSelected(p);
            break;
        case SELECTIONMODE::Line:
            m_selectedPoints=QVector<QPointF>{p,p};
            break;
        case SELECTIONMODE::LinesVOrdered:
            addSelectionVOrderPoint(p);
            break;
        case SELECTIONMODE::Lines:
            addSelectionLinesPoint( p );
            break;
        case SELECTIONMODE::Polygon:
            addSelectionPolygonPoint( p );
            break;
        default:
            break;
        }
    }
    else if( event->button()==Qt::RightButton &&
             ( event->modifiers()==Qt::ShiftModifier ||  m_mouseMode==MouseMode::Select || m_mouseMode==MouseMode::Pick  ) ){

        switch(m_selectionMode){
        case SELECTIONMODE::Lines:
            emit linesSelected(selectionPolygon());
            clearSelection();
             m_selectionActive=false;
            break;
        case SELECTIONMODE::Polygon:
            emit polygonSelected(selectionPolygon());
            clearSelection();
             m_selectionActive=false;
            break;
        default:
            break;
        }
    }
    else if( ( event->button()==Qt::LeftButton && m_mouseMode==MouseMode::DeletePick ) ||
                (event->button()==Qt::MiddleButton &&
                  (event->modifiers()==Qt::ShiftModifier ||
                        m_mouseMode==MouseMode::Select || m_mouseMode==MouseMode::Pick) ) ){
        m_selectionActive=true;
        emit removePoint(p);
        /*
        m_selectionActive=true;

        switch(m_selectionMode){
        case SELECTIONMODE::SinglePoint:
            m_selectedPoints=QVector<QPointF>{p};
            emit removePoint(p);
            break;
        case SELECTIONMODE::Polygon:
            removeSelectionPoint(p);
            break;
        case SELECTIONMODE::LinesVOrdered:
            removeSelectionPoint(p);
            break;
        default:
            break;
        }
        */
    }
     event->accept();

}


void AxisView::mouseReleaseEvent(QMouseEvent * event){

    const int ZOOM_PIXEL_THRESHOLD = 3;

    // zoom on ctrl + left button
    if( event->button() == Qt::LeftButton && m_zoomActive ){

        auto selStopPix=event->pos();
        //selStopPix-=viewport()->pos();  // YYY
        auto selStopScene=mapToScene(selStopPix);
        QPointF selStopAxis( m_xAxis->toAxis(selStopScene.x()), m_zAxis->toAxis(selStopScene.y()));

        QPointF selStartScene( m_xAxis->toScene(m_selStart.x()), m_zAxis->toScene(m_selStart.y()));
        auto selStartPix=mapFromScene(selStartScene);
        //selStartPix+=viewport()->pos();

        auto deltaPix=selStopPix - selStartPix;

        qreal x0, x1;
        qreal z0, z1;
        m_xAxis->setSelection(0,0); // hide rubberband
        m_zAxis->setSelection(0,0); // hide rubberband

        switch (m_zoomMode) {
        case ZOOMMODE::X:
            if(std::abs(deltaPix.x())<ZOOM_PIXEL_THRESHOLD){
                x0=m_xAxis->min(); x1=m_xAxis->max();
            }
            else{
                x0=m_selStart.x(); x1=selStopAxis.x();
            }
            z0=m_zAxis->minView();
            z1=m_zAxis->maxView();
            break;
        case ZOOMMODE::Z:
            if(std::abs(deltaPix.y())<ZOOM_PIXEL_THRESHOLD ){
                z0=m_zAxis->min(); z1=m_zAxis->max();
            }
            else{
                z0=m_selStart.y(); z1=selStopAxis.y();
            }
            x0=m_xAxis->minView();
            x1=m_xAxis->maxView();
            break;
        case ZOOMMODE::BOTH:

            if(std::sqrt(deltaPix.x()*deltaPix.x() + deltaPix.y()*deltaPix.y())<ZOOM_PIXEL_THRESHOLD ){
                x0=m_xAxis->min(); x1=m_xAxis->max();
                z0=m_zAxis->min(); z1=m_zAxis->max();
            }
            else{
                x0=m_selStart.x(); x1=selStopAxis.x();
                z0=m_selStart.y(); z1=selStopAxis.y();
            }
            break;
        default:
            break;
        }

        if(x0>x1) std::swap(x0,x1);
        if(z0>z1) std::swap(z0,z1);
        zoom( QRectF(x0,z0,x1-x0,z1-z0));

        m_zoomActive=false;
    }
    //else if( event->button()==Qt::LeftButton &&
    //         ( event->modifiers()==Qt::ShiftModifier ||  m_mouseMode==MouseMode::Select ||
    //           m_mouseMode==MouseMode::Pick || m_mouseMode==MouseMode::DeletePick) ){
    else if( event->button()==Qt::LeftButton && m_selectionActive){
        m_selectionActive=false;
        if( m_selectionMode==SELECTIONMODE::Line){
            emit lineSelected(selectedLine());
        }
    }

}


void AxisView::mouseMoveEvent(QMouseEvent * event){

    QPointF scenePos=mapToScene(event->pos());
    QPointF p=toAxis(scenePos);
    emit mouseOver(p);

    m_xAxis->setCursorPosition(p.x());
    m_zAxis->setCursorPosition(p.y());

    // zoom on ctrl + left button
    if(  m_zoomActive){

        switch (m_zoomMode) {
        case ZOOMMODE::X:
            m_xAxis->setSelection(m_selStart.x(), p.x() );
            break;
        case ZOOMMODE::Z:
            m_zAxis->setSelection(m_selStart.y(), p.y());
            break;
        case ZOOMMODE::BOTH:
            m_xAxis->setSelection(m_selStart.x(), p.x() );
            m_zAxis->setSelection(m_selStart.y(), p.y());
            break;
        default:
            break;
        }
    }
    else if( m_selectionActive ){
        if(m_selectionMode==SELECTIONMODE::Line){
            if( m_selectedPoints.size()>1) m_selectedPoints[1]=p;
            invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
            scene()->update();
        }
        //else if( m_mouseMode==MouseMode::Pick && m_selectionMode==SELECTIONMODE::SinglePoint ){
            //m_selectedPoints=QVector<QPointF>{p};
            //emit pointSelected(p);	// this interferes with top picking, if needed use multiply point mode
        //}
        else if( m_selectionActive && m_mouseMode==MouseMode::DeletePick ){
            emit removePoint(p);
        }
    }

    event->accept();
}


void AxisView::mouseDoubleClickEvent(QMouseEvent* event){

    m_selectedPoints.clear();
    scene()->clearSelection();

    QGraphicsItem* item=itemAt(event->pos());
    if( item ){
        item->setSelected(true);
    }

    scene()->update();

    event->accept();
}



void AxisView::keyPressEvent(QKeyEvent * event){

    if( event->key()!=Qt::Key_Escape ){
        QWidget::keyPressEvent(event);      // not using this event, call base to further distribute it!!
        return;
    }

    if( m_selectionMode==SELECTIONMODE::Line){
        m_selectedPoints.clear();
        invalidateScene(sceneRect(),QGraphicsScene::ForegroundLayer);
        scene()->update();
    }
    else if( m_selectionMode==SELECTIONMODE::Polygon ){
        m_selectedPoints.clear();
        scene()->clearSelection();
        scene()->update();
    }

    // now the event is consumed!
}

void AxisView::leaveEvent(QEvent *){
    // hide cursor
    m_xAxis->setCursorPosition(Axis::NO_CURSOR);
    m_zAxis->setCursorPosition(Axis::NO_CURSOR);
}

std::ostream& operator<<(std::ostream& os, const QRectF& r){
    os<<"RECT: x="<<r.x()<<" y="<<r.y()<<" w="<<r.width()<<" h="<<r.height();
    return os;
}

void AxisView::resizeEvent(QResizeEvent *){

    m_xAxis->setViewPixelLength(viewport()->width());
    m_zAxis->setViewPixelLength(viewport()->height());

    if( scene() ){
        QRectF r=viewRectFromAxes();
        std::cout<<"VR from axes: "<<r<<std::endl;
        r=adjustViewRect(r);
        std::cout<<"adjusted: "<<r<<std::endl;
        fitInView(r, Qt::IgnoreAspectRatio  );
        auto x0=r.x();
        auto x1=x0+r.width();
        auto z0=r.y();
        auto z1=z0+r.height();
        std::cout<<"x0="<<x0<<" x1="<<x1<<std::endl;
        std::cout<<"z0="<<z0<<" z1="<<z1<<std::endl;
        m_xAxis->setViewRange(x0,x1);
        m_zAxis->setViewRange(z0,z1);
    }



}


void AxisView::drawBackground(QPainter *painter, const QRectF &rect){

    QGraphicsView::drawBackground(painter, rect);

    painter->save();

    // add mesh
    QPen mainTickPen(Qt::gray, 0 );
    QPen subTickPen(Qt::lightGray, 0);

    if( m_xMesh){
        auto xticks=xAxis()->computeTicks( xAxis()->toAxis(rect.left()), xAxis()->toAxis(rect.right()) );
        for( auto tick : xticks){
            painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
            qreal x=xAxis()->toScene(tick.value);
            painter->drawLine ( QPointF(x, rect.top()), QPointF(x, rect.bottom())); // fp version
        }
    }

    if( m_zMesh){
        auto zticks=zAxis()->computeTicks( zAxis()->toAxis(rect.top()), zAxis()->toAxis(rect.bottom()) );
        for( auto tick : zticks){
            painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
            qreal z=zAxis()->toScene(tick.value);
            painter->drawLine(QPointF(rect.left(), z), QPointF(rect.right(), z));   // fp version
        }
    }

    painter->restore();
}


void AxisView::drawForeground(QPainter *painter, const QRectF &rectInScene){

    painter->save();
    painter->setWorldMatrixEnabled(false);
    QPen selectionPen(Qt::red,2);
    selectionPen.setCosmetic(true);
    painter->setPen(selectionPen);

    switch(m_selectionMode){

    case SELECTIONMODE::Line:{
        auto line=selectedLine();
        QLineF scline(toScene(line.p1()), toScene(line.p2()));
        painter->drawLine(mapFromScene(scline.p1()), mapFromScene(scline.p2()));
        break;
    }
    case SELECTIONMODE::Polygon:
    case SELECTIONMODE::Lines:
    case SELECTIONMODE::LinesVOrdered:
    {
        QPolygonF scenePolygon;
        for( auto p : selectionPolygon()){
            scenePolygon<<toScene(p);
        }
        QPolygon polygonInView=mapFromScene(scenePolygon);
        for( auto piv : polygonInView){
            painter->drawRect(piv.x()-1, piv.y()-1, 3, 3);
        }
        if(m_selectionMode==SELECTIONMODE::Polygon){
            painter->drawPolygon(polygonInView);
        }
        else{
            painter->drawPolyline(polygonInView);
        }
        break;
    }
    default:
        break;
    }

    painter->setWorldMatrixEnabled(true);
    painter->restore();

    QGraphicsView::drawForeground( painter, rectInScene );
}


// only add point if new side does not intersect others, points are in axis/content coords
void AxisView::addSelectionPolygonPoint( QPointF p ){

    if( m_selectedPoints.size()>2 ){


        QLineF pointToFirst( p, m_selectedPoints.front() );
        QLineF lastToPoint( m_selectedPoints.back(), p );

        for( int i=1; i<m_selectedPoints.size(); i++){

            QLineF side( m_selectedPoints.at(i-1), m_selectedPoints.at(i));
            QPointF inter;

            if( side.intersect(pointToFirst, &inter)==QLineF::BoundedIntersection && inter!=m_selectedPoints.first()){
                return;
            }

            if( i+1<m_selectedPoints.size() && side.intersect(lastToPoint, &inter)==QLineF::BoundedIntersection &&
                    inter!=m_selectedPoints.last()){
                return;
            }
        }

    }

    m_selectedPoints<<p;

    updateSceneSelection();

    //emit polygonSelected(selectionPolygon());

    //scene()->update();

    invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
    scene()->update();
}


void AxisView::addSelectionLinesPoint( QPointF p ){
    m_selectedPoints<<p;
    updateSceneSelection();
    invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
    scene()->update();
}

void AxisView::updateSceneSelection(){
    //if( m_selectedPoints.size()>2){

            QPolygonF poly;     // this is in scene coords
            for( auto psel:m_selectedPoints){
                auto psc=toScene(psel);
                poly<<psc;
            }

            QPainterPath selectionArea;
            selectionArea.addPolygon(poly);//m_selectionPolygon);
            scene()->setSelectionArea(selectionArea,viewportTransform());
      //  }
}

// inserts point based on y value, if y already exists do noting
void AxisView::addSelectionVOrderPoint(QPointF p){
    if( m_selectedPoints.empty()){
        m_selectedPoints.push_back(p);
    }
    else if(p.y()<m_selectedPoints.front().y()){
        m_selectedPoints.push_front(p);
    }
    else if(p.y()>m_selectedPoints.back().y()){
        m_selectedPoints.push_back(p);
    }
    else{
        for( auto it=m_selectedPoints.begin(); it!=m_selectedPoints.end(); ++it){
            if( it->y()==p.y()) return;     // y value already exists, skip
            if( it->y()>p.y()){             // must insert before
                m_selectedPoints.insert(it, p);
                break;
            }
        }
    }

    invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
    scene()->update();
}

void AxisView::removeSelectionPoint(QPointF p){

    if( m_selectedPoints.empty()){
        return;
    }
    // find closest point
    int minidx=0;
    qreal mindist2=std::numeric_limits<qreal>::max();
    for( int i=0; i<m_selectedPoints.size(); i++){
        auto pp=m_selectedPoints[i]-p;
        auto dist2=pp.x()*pp.x()+pp.y()*pp.y();
        if(dist2<mindist2){
            minidx=i;
            mindist2=dist2;
        }
    }
    m_selectedPoints.remove(minidx);

    updateSceneSelection();

    invalidateScene(sceneRect(), QGraphicsScene::ForegroundLayer);
    scene()->update();
}


QPointF AxisView::toAxis(QPointF p){

    if( !xAxis() || !zAxis() ) return p;

    return QPointF(xAxis()->toAxis(p.x()), zAxis()->toAxis(p.y()));
}


QPointF AxisView::toScene(QPointF p){

    if( !xAxis() || !zAxis() ) return p;

    return QPointF(xAxis()->toScene(p.x()), zAxis()->toScene(p.y()));
}

void AxisView::updateBackground(){
  //  if(scene()){
    //    scene()->update();
    //}
    invalidateScene(sceneRect(), QGraphicsScene::BackgroundLayer);
    resetCachedContent();
    if(scene()) scene()->update();

}
