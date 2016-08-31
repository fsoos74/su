#include "rulergraphicsview.h"

#include<iostream>
#include<QScrollBar>
#include<QGraphicsItem>
#include<QEvent>
#include<QMouseEvent>
#include<QLineF>
#include<QDebug>

#include<cmath>

RulerGraphicsView::RulerGraphicsView(QWidget *parent) : QGraphicsView(parent){

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    setViewportMargins( m_RULER_WIDTH, m_RULER_HEIGHT, 0, 0);

    m_leftRuler=new GVRuler( this, GVRuler::VERTICAL_RULER);
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), m_leftRuler, SLOT( update()) );
    connect( verticalScrollBar(), SIGNAL(rangeChanged(int,int)), m_leftRuler, SLOT( update()) );

    m_topRuler=new GVRuler( this, GVRuler::HORIZONTAL_RULER);
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), m_topRuler, SLOT( update()) );
    connect( horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), m_topRuler, SLOT( update()) );

    setDragMode(QGraphicsView::RubberBandDrag);

    viewport()->installEventFilter(this);
    m_leftRuler->installEventFilter(this);
    m_topRuler->installEventFilter(this);

}


void RulerGraphicsView::setGridMode( RulerGraphicsView::GridMode m ){

    if( m==m_gridMode) return;

    m_gridMode=m;

    update();
}

void RulerGraphicsView::setRulerWidth( int w ){

    if( m_RULER_WIDTH==w ) return;

    m_RULER_WIDTH=w;

    updateLayout();
}

void RulerGraphicsView::setRulerHeight( int h ){

    if( h==m_RULER_HEIGHT) return;

    m_RULER_HEIGHT=h;

    updateLayout();
}

void RulerGraphicsView::setSelectionPolygon( QPolygonF poly){

    if( poly==m_selectionPolygon) return;

    m_selectionPolygon=poly;

    scene()->update();
  //  update();
}

void RulerGraphicsView::zoomFitWindow(){

    if( !scene() ) return;

    fitInView( scene()->sceneRect());
    m_leftRuler->update();
    m_topRuler->update();
}

void RulerGraphicsView::zoomBy( qreal factor ){

     if( !scene() ) return;

    if( factor*m_zoomFactor>1){

        scale(factor, factor);
        m_zoomFactor*=factor;
        m_leftRuler->update();
        m_topRuler->update();
    }
    else{
        zoomFitWindow();
    }
}

void RulerGraphicsView::setSelectionMode(SelectionMode  mode){

    if( mode==m_selectionMode) return;

    m_selectionMode=mode;
}

void RulerGraphicsView::resizeEvent(QResizeEvent *){
/*
    QRect mappedSceneRect=mapFromScene(sceneRect()).boundingRect();

    if( viewport()->rect().width()>mappedSceneRect.width() ||
            viewport()->rect().height()>mappedSceneRect.height() ){
        fitInView(sceneRect());
    }
  */


    if( scene() ){
        QRectF bounds = sceneRect();//scene()->itemsBoundingRect();
        //bounds.setWidth(bounds.width()*0.9);         // to tighten-up margins
        //bounds.setHeight(bounds.height()*0.9);       // same as above
        fitInView(bounds);
        centerOn(bounds.center());
    }

    m_topRuler->update();
    m_leftRuler->update();


    updateLayout();

}

// only add point if new side does not intersect others
void RulerGraphicsView::addSelectionPolygonPoint( QPointF p ){

    if( m_selectionPolygon.size()>2 ){


        QLineF pointToFirst( p, m_selectionPolygon.first() );
        QLineF lastToPoint( m_selectionPolygon.last(), p );

        for( int i=1; i<m_selectionPolygon.size(); i++){

            QLineF side( m_selectionPolygon.at(i-1), m_selectionPolygon.at(i));
            QPointF inter;

            if( side.intersect(pointToFirst, &inter)==QLineF::BoundedIntersection && inter!=m_selectionPolygon.first()){
                return;
            }

            if( i+1<m_selectionPolygon.size() && side.intersect(lastToPoint, &inter)==QLineF::BoundedIntersection && inter!=m_selectionPolygon.last()){
                return;
            }
        }

    }

    m_selectionPolygon<<p;

    if( m_selectionPolygon.size()>2){

        QPainterPath selectionArea;
        selectionArea.addPolygon(m_selectionPolygon);
        scene()->setSelectionArea(selectionArea,viewportTransform());
    }

    scene()->update();

}


void RulerGraphicsView::mousePressEvent(QMouseEvent* event){

    if( event->modifiers()!=Qt::ShiftModifier) return;

    if( m_selectionMode==SelectionMode::Polygon){

        QPointF scenePos=mapToScene(event->pos());
        addSelectionPolygonPoint( scenePos );
    }

    event->accept();
}

void RulerGraphicsView::mouseMoveEvent(QMouseEvent* event){

    // call this for hoover events being handles correctly
    QGraphicsView::mouseMoveEvent(event);

    QPointF scenePos=mapToScene(event->pos());

    emit mouseOver( scenePos);


}

void RulerGraphicsView::mouseDoubleClickEvent(QMouseEvent* event){

  //  if( event->modifiers()!=Qt::ShiftModifier) return;

    m_selectionPolygon.clear();
    scene()->clearSelection();

    QGraphicsItem* item=itemAt(event->pos());
    if( item ){
        item->setSelected(true);
    }

    scene()->update();

    /*
    // clear selection
    if( m_selectionMode==SelectionMode::Polygon){
        m_selectionPolygon.clear();
        scene()->clearSelection();
        scene()->update();
    }
    */
    event->accept();
}

void RulerGraphicsView::keyPressEvent(QKeyEvent * event){

    if( m_selectionMode==SelectionMode::Polygon && event->key()==Qt::Key_Escape ){
        m_selectionPolygon.clear();
        scene()->clearSelection();
        scene()->update();
    }
}

QPoint RulerGraphicsView::mouseEventPosToViewport( QPoint pos, bool start){

    if( m_mouseSelectionWidget==m_leftRuler){
        return QPoint( (start)? 0 : viewport()->width(), pos.y());
    }
    else if( m_mouseSelectionWidget==m_topRuler){
        return QPoint( pos.x(), (start)? 0 : viewport()->height());
    }
    else if( m_mouseSelectionWidget==viewport()){
        return pos;
    }
    qFatal("Unexpected widget in RulerGraphicsView::mouseEventPosToViewport ");
}

template<class Rect>
void dumpRect( Rect r ){
    std::cout<<"x="<<r.x()<<" y="<<r.y()<<" w="<<r.width()<<" h="<<r.height()<<std::endl;
}

bool RulerGraphicsView::eventFilter(QObject *obj, QEvent *ev){

    QWidget* widget=dynamic_cast<QWidget*>(obj);
    QMouseEvent* mouseEvent=dynamic_cast<QMouseEvent*>(ev);

    if( !widget || !mouseEvent ) return QObject::eventFilter(obj,ev);

    if( !(widget==viewport() || widget==m_leftRuler || widget==m_topRuler)) return QObject::eventFilter(obj,ev);

    if (mouseEvent->modifiers() != Qt::ControlModifier && !m_mouseSelection ) return QObject::eventFilter(obj,ev);

    // zoom

    if( mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->button()==Qt::LeftButton ){

        m_mouseSelection=true;
        m_mouseSelectionWidget=widget;
        m_mouseSelectionStart=mouseEventPosToViewport(mouseEvent->pos(), true);
        if( !m_rubberBand ){
            m_rubberBand=new QRubberBand( QRubberBand::Shape::Rectangle, viewport());
        }
        m_rubberBand->setGeometry(QRect(m_mouseSelectionStart, QSize()));
        m_rubberBand->show();
    }

    else if( mouseEvent->type()==QEvent::MouseMove && m_mouseSelection ){

        if( widget==m_mouseSelectionWidget ){

            QPoint cur=mouseEventPosToViewport(mouseEvent->pos(), false);
            m_rubberBand->setGeometry(QRect(m_mouseSelectionStart, cur).normalized());

        }

    }
    else if( mouseEvent->type()==QEvent::MouseButtonRelease && mouseEvent->button()==Qt::LeftButton){

        m_mouseSelection=false;
       // mouseSelectionWidget=nullptr;
        QPoint end=mouseEventPosToViewport(mouseEvent->pos(), false);
        m_rubberBand->hide();

        setDragMode(QGraphicsView::RubberBandDrag);

        QRect selectedRectInViewport=QRect(m_mouseSelectionStart, end).normalized();
        QRectF selectedRectInScene=mapToScene( selectedRectInViewport ).boundingRect();

        // adjust for unzooming
        QPoint diff=m_mouseSelectionStart - mouseEventPosToViewport(mouseEvent->pos(), true); // convert as start for distance computation
        if( std::abs(diff.x())<MINIMUM_ZOOM_PIXEL && m_mouseSelectionWidget!=m_leftRuler){

            selectedRectInScene=QRectF(sceneRect().x(), selectedRectInScene.y(), sceneRect().width(), selectedRectInScene.height());
        }
        if( std::abs(diff.y())<MINIMUM_ZOOM_PIXEL && m_mouseSelectionWidget!=m_topRuler ){
           selectedRectInScene=QRectF( selectedRectInScene.x(), sceneRect().y(),
                                       selectedRectInScene.width(), sceneRect().height()).normalized();
        }

        selectedRectInScene=selectedRectInScene.intersected(sceneRect());

        fitInView(selectedRectInScene );//.normalized());
        m_leftRuler->update();
        m_topRuler->update();
    }


    ev->accept();// return QObject::eventFilter(obj,ev);
    return true;
}


void RulerGraphicsView::drawGrid(QPainter* painter, const QRect &rectInView){

    painter->save();

    painter->setWorldMatrixEnabled(false);

    QPen thePen(Qt::black);
    thePen.setCosmetic(true);
    painter->setPen(thePen);

    QVector< GVRuler::Tick > leftTicks=m_leftRuler->computeTicks();
    for( GVRuler::Tick tick : leftTicks){

        painter->drawLine( rectInView.left(), tick.coord(), rectInView.right(), tick.coord() );//rectInScene.left(), tick.value(), rectInScene.right(), tick.value());
    }

    QVector< GVRuler::Tick > topTicks=m_topRuler->computeTicks();
    for( GVRuler::Tick tick : topTicks){

        painter->drawLine( tick.coord(), rectInView.top(), tick.coord(), rectInView.bottom() );
    }

    painter->setWorldMatrixEnabled(true);

    painter->restore();
}


void RulerGraphicsView::drawBackground(QPainter *painter, const QRectF &rectInScene){

     QGraphicsView::drawBackground( painter, rectInScene );


    if( m_gridMode == GridMode::Background){
        QRect rectInView=mapFromScene(rectInScene).boundingRect();
        drawGrid(painter, rectInView);
    }
}


void RulerGraphicsView::drawForeground(QPainter *painter, const QRectF &rectInScene){

    if( m_gridMode == GridMode::Foreground){
        QRect rectInView=mapFromScene(rectInScene).boundingRect();
        drawGrid(painter, rectInView);
    }

    painter->setWorldMatrixEnabled(false);
    painter->setPen(QPen(Qt::red,0));

    QPolygon polygonInView=mapFromScene(m_selectionPolygon);
    painter->drawPolygon(polygonInView);
    painter->setWorldMatrixEnabled(true);


    QGraphicsView::drawForeground( painter, rectInScene );

}

void RulerGraphicsView::updateLayout(){

    setUpdatesEnabled( false );

    setViewportMargins( m_RULER_WIDTH, m_RULER_HEIGHT, 0, 0);

    m_leftRuler->setGeometry( 0, viewport()->y(), m_RULER_WIDTH, viewport()->height() );
    m_topRuler->setGeometry( viewport()->x(), 0, viewport()->width(), m_RULER_HEIGHT );

    setUpdatesEnabled( true );
}






GVRuler::GVRuler( RulerGraphicsView* parent, Orientation orient ):QWidget(parent), m_view(parent), m_orientation( orient){
}

void GVRuler::setAnnotationFunction( std::function<QString(qreal)> func ){

    m_annotationFunction=func;

    update();
}

void GVRuler::setLabel( const QString& str){

    if( str==m_label) return;

    m_label=str;

    update();
}

void GVRuler::setMinimumValueIncrement(qreal incr){

    if( incr==m_minimumValueIncrement) return;
    m_minimumValueIncrement = incr;
    update();
}

void GVRuler::setMinimumPixelIncrement(int incr){

    if( incr==m_minimumPixelIncrement) return;
    m_minimumPixelIncrement=incr;
    update();
}

void GVRuler::setTickMarkSize(int s){

    if( s==m_TICK_MARK_SIZE) return;
    m_TICK_MARK_SIZE=s;
    update();
}

void GVRuler::paintEvent(QPaintEvent*){

    QPainter painter(this);

    painter.setPen(Qt::black);

    if( m_orientation==VERTICAL_RULER){
        drawVertical( painter );
    }
    else{
        drawHorizontal( painter );
    }

}



qreal GVRuler::tickIncrement(int size_pix, qreal size_data)const{

    qreal pix_per_unit=std::fabs( qreal(size_pix)/size_data );
    qreal incr= std::pow(10,std::floor(std::log10( m_minimumPixelIncrement/pix_per_unit)));   //

    if( incr * pix_per_unit>=m_minimumPixelIncrement){
        ;;
    }
    else if( 2*incr*pix_per_unit>=m_minimumPixelIncrement){
        incr*=2;
    }
    else  if( 5*incr*pix_per_unit>=m_minimumPixelIncrement){
        incr*=5;
    }
    else  if( 10*incr*pix_per_unit>=m_minimumPixelIncrement){
        incr*=10;
    }
    else{

        qWarning("No tick increment found!");


        incr=-1;
    }

    if( incr<m_minimumValueIncrement) incr=m_minimumValueIncrement;

    return incr;
}

QVector< GVRuler::Tick > GVRuler::computeTicks()const{

    QVector< Tick > ticks;
    QRect mappedSceneRect=m_view->mapFromScene(m_view->sceneRect()).boundingRect();

    if( m_orientation==VERTICAL_RULER){

       QPoint startP( 0, mappedSceneRect.top());
       QPoint endP( 0, mappedSceneRect.bottom());
       qreal incr=tickIncrement( mappedSceneRect.height(), m_view->sceneRect().height() );

       qreal startVal=m_view->mapToScene( startP ).y();
       qreal endVal=m_view->mapToScene(endP).y();


       int first=std::ceil(startVal/incr);
       int last=std::floor(endVal/incr);
       if( first>last) std::swap(first, last);  // if values decrease for increasing pixel coordinate

       for( int i=first; i<=last; i++){

           qreal value=i*incr;
           int pix=std::round( m_view->mapFromScene( QPointF( 0, value )).y() );
           ticks.push_back( Tick( pix, value ));
       }
    }

    else{                                                                   // Horizontal ruler

        QPoint startP( mappedSceneRect.left(), 0);
        QPoint endP( mappedSceneRect.right(), 0);
        qreal incr=tickIncrement( mappedSceneRect.width(), m_view->sceneRect().width() );

        qreal startVal=m_view->mapToScene( startP ).x();
        qreal endVal=m_view->mapToScene(endP).x();

        int first=std::floor(startVal/incr);
        int last=std::ceil(endVal/incr);
        if( first>last) std::swap(first, last);  // if values decrease for increasing pixel coordinate
        for( int i=first; i<=last; i++){

            qreal value=i*incr;
            int pix=std::round( m_view->mapFromScene( QPointF( value, 0 )).x() );
            ticks.push_back( Tick( pix, value ));
        }


     }

    return ticks;

}


void GVRuler::drawVertical( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    const QString& text = m_label;

    painter.save();
    painter.rotate( -90);
   // painter.translate( 0, -width());
    QSize text_size=painter.fontMetrics().size( Qt::TextSingleLine, text);
    painter.drawText( -( height() + text_size.width() )/2, text_size.height(), text );         // center vertically
    painter.restore();


    QVector<Tick> ticks=computeTicks();

    int x1=width();
    int x2=x1-1.5*m_TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int y=tick.coord();
        painter.drawLine( x1, y , x2, y );

        QString annotation=m_annotationFunction(tick.value());//QString::number( tick.value(), 'g', 4);
        QSize annotation_size=painter.fontMetrics().size( Qt::TextSingleLine, annotation);
        int halfSize=annotation_size.height()/2;

        if( y<halfSize || y+halfSize>=height() ) continue;

        painter.drawText( 0, y - m_minimumPixelIncrement/2, x2, m_minimumPixelIncrement, Qt::AlignRight | Qt::AlignVCenter, annotation);

    }

}

void GVRuler::drawHorizontal( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    const QString& text = m_label;
    QSize text_size=painter.fontMetrics().size( Qt::TextSingleLine, text);
    painter.drawText( ( width() - text_size.width() ) / 2, text_size.height(), text );  // center horizontally


    QVector<Tick> ticks=computeTicks();
    int y1=height();
    int y2=y1-1.5*m_TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int x=tick.coord();
        painter.drawLine( x, y1, x, y2 );

        QString annotation=m_annotationFunction(tick.value());//QString::number(tick.value(), 'g', 4);
        QSize annotation_size=painter.fontMetrics().size( Qt::TextSingleLine, annotation);
        int halfSize=annotation_size.width()/2;
        if( x<halfSize || x+halfSize>=width() ) continue;

        painter.drawText( x - m_minimumPixelIncrement/2, 0, m_minimumPixelIncrement, y2, Qt::AlignBottom|Qt::AlignHCenter, annotation);
    }

}
