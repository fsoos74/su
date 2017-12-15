#include "gatherview.h"
#include "gatherlabel.h"
#include "gatherruler.h"
#include "axxislabelwidget.h"
#include "headerui.h"
#include <colortable.h>

#include<QScrollBar>
#include<iostream>
#include<QApplication>
#include<chrono>
#include<QResizeEvent>


GatherView::GatherView( QWidget* parent) : QScrollArea(parent)
{
    m_gatherLabel=new GatherLabel(this);
    m_gatherLabel->setBackgroundRole(QPalette::Base);
    m_gatherLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //setWidgetResizable(true);// !!!
    m_gatherLabel->setMouseTracking(true);

    setWidget( m_gatherLabel);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    connect( this, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)),
             m_gatherLabel, SLOT( onViewGatherChanged(std::shared_ptr<seismic::Gather>)));

    setViewportMargins( RULER_WIDTH, RULER_HEIGHT, 0, 0);

    m_leftRuler=new GatherRuler( this, GatherRuler::VERTICAL_RULER );
    m_leftRuler->setMouseTracking(true);
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), m_leftRuler, SLOT( update()) );
    connect( this, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)), m_leftRuler, SLOT(update()));
    //connect( this, SIGNAL(pixelPerSecondChanged(qreal)),  m_gatherLabel, SLOT(updatePixmap()));

    m_topRuler=new GatherRuler( this, GatherRuler::HORIZONTAL_RULER );
    m_topRuler->setMouseTracking(true);
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), m_topRuler, SLOT( update()) );
    connect( this, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)), m_topRuler, SLOT(update()));
    //connect( this, SIGNAL(pixelPerTraceChanged(qreal)), m_gatherLabel, SLOT(updatePixmap()));

    m_axxisLabelWidget=new AxxisLabelWidget(this);

    m_gatherLabel->installEventFilter(this);
    m_leftRuler->installEventFilter(this);
    m_topRuler->installEventFilter(this);


      updateGeometry();

      setPixelPerUnits( 10, 100);

      m_gatherScaler=new GatherScaler(this);

      connect( m_gatherScaler, SIGNAL(changed()), m_gatherLabel, SLOT(updateTraceScaleFactors()));

      // setup picker
      m_picker=new Picker(this);
      connect( m_picker, SIGNAL(picksChanged()), gatherLabel(), SLOT(update()) );
      connect( this, SIGNAL(gatherChanged(std::shared_ptr<seismic::Gather>)), m_picker, SLOT(setGather(std::shared_ptr<seismic::Gather>)) );

}


void GatherView::setGather( std::shared_ptr<seismic::Gather> g){

    if( g==m_gather) return;

    m_gather=g;

    buildTraceLookup();

    updateTimeRange();

    // update pixel pixels per trace rather than resizing
    if( m_gather ){
        //m_gatherLabel->setFixedSize( m_pixelPerTrace*m_gather->size(), m_pixelPerSecond*seconds() );
        setPixelPerTrace( qreal(m_gatherLabel->width())/m_gather->size());
    }


    emit gatherChanged(m_gather); // important to do this after the pixel per trace is updated!!!
/*
    if( m_gather && m_gather->size()>0 && m_gatherLabel->highlightedTrace()>=m_gather->size() ){
        m_gatherLabel->setHighlightedTrace(m_gather->size()-1);
    }
*/


    //normalize();
    //update();

}

void GatherView::setHighlightedPoints(QVector<SelectionPoint> points){
    m_highlightedPoints=points;
    m_gatherLabel->update();
}

void GatherView::setIntersectionTraces(QVector<int> trc){

    if( trc==m_intersectionTraces) return;

    m_intersectionTraces=trc;

    m_gatherLabel->update();
}


void GatherView::setIntersectionTimes(QVector<qreal> t){

    if( t==m_intersectionTimes ) return;

    m_intersectionTimes=t;

    m_gatherLabel->update();
}

void GatherView::setPrimarySortKey(GatherSortKey k){

    if( k==m_primarySortKey ) return;

    m_primarySortKey=k;

    buildTraceLookup();

    emit primarySortKeyChanged(k);
}


void GatherView::buildTraceLookup(){

    m_traceLookup.clear();

    if( !m_gather ) return;

    for( int i=0; i<m_gather->size(); i++){

        int iline=(*m_gather)[i].header().at("iline").intValue();
        int xline=(*m_gather)[i].header().at("xline").intValue();

        QString key=QString("%1_%2").arg(iline).arg(xline);

        m_traceLookup.insert( key, i );
    }
}

// return trace number or -1 if not found
int GatherView::lookupTrace(int iline, int xline){

    QString key=QString("%1_%2").arg(iline).arg(xline);

    return m_traceLookup.value(key, -1);
}

void GatherView::setCursorPosition(SelectionPoint p){

    if( p==m_cursorPosition ) return;

    m_cursorPosition=p;

    m_gatherLabel->update();

    int trcno=lookupTrace(p.iline, p.xline);
    topRuler()->setCurrentPos(trcno);
    leftRuler()->setCurrentPos( p.time );

}

void GatherView::setMouseMode(MouseMode m){

    if( m==m_mouseMode ) return;

    m_mouseMode=m;

    m_gatherLabel->setCursor(modeCursor(m_mouseMode));

    emit mouseModeChanged(m_mouseMode);
}

std::shared_ptr<Grid2D<float> > GatherView::horizon(QString name)const{

    // return null pointer if not exists
    if( !m_horizons.contains(name)){
        return std::shared_ptr< Grid2D<float> >();
    }

    return m_horizons[name];
}

QColor GatherView::horizonColor(QString name){
    return m_horizonColors.value(name);
}

void GatherView::addHorizon( QString name, std::shared_ptr<Grid2D<float> > g, QColor color){

    m_horizons[name]=g;
    m_horizonColors[name]=color;
    m_gatherLabel->update();
}


void GatherView::removeHorizon( QString name){

    if( m_horizons.contains(name)){
        m_horizons.remove(name);
        m_gatherLabel->update();
    }
}

QStringList GatherView::horizonList(){

   return m_horizons.keys();
}

std::shared_ptr< Volume > GatherView::volume()const{

    return m_volume;
}

void GatherView::setVolume( std::shared_ptr<Volume> volume){    // set null for closing

    m_volume=volume;
    m_gatherLabel->updateBuffers();
  //  m_gatherLabel->update();
}

QStringList GatherView::traceAnnotation( size_t traceNumber )const{

    if( !m_gather ) return QStringList();

    if( traceNumber>=m_gather->size()){
       return QStringList("N/A");
    }

    QStringList annos;
    for( auto key : m_annotationKeys){
        seismic::Trace& trc=(*m_gather)[traceNumber];
        annos.push_back( toQString( trc.header().at( key ) ) );
    }

    return annos;
}


 void GatherView::setAnnotationKeys( const std::vector<std::string>& keys ){

     m_annotationKeys=keys;

     m_topRuler->update();
 }

void GatherView::setPixelPerTrace( qreal ppt ){

    if( m_fixedScale) return;

    if( ppt==m_pixelPerTrace) return;

    if( !m_gather || m_gather->empty() ) return;

    qreal oldPPT=m_pixelPerTrace;

    int newWidth=ppt*m_gather->size();
/* allow label to become smaller than viewport
    if( newWidth<viewport()->width() ){
        newWidth=viewport()->width();
        ppt=qreal(newWidth)/m_gather->size();
    }
*/
    m_pixelPerTrace=ppt;

    emit( pixelPerTraceChanged(ppt) );

    m_gatherLabel->setFixedWidth(newWidth);
    //m_gatherLabel->resize( newWidth, height());

    qreal factor=ppt/oldPPT;

    adjustScrollBar( horizontalScrollBar(), factor);

    m_topRuler->update();
}

void GatherView::setPixelPerSecond( qreal pps ){

    if( m_fixedScale) return;

    if( pps==m_pixelPerSecond) return;

    qreal secs=seconds();

    qreal oldPPS=m_pixelPerSecond;

    if( secs<=0 ) return;

    int newHeight=pps*secs;
/* allow label to become smaller than viewport
    if( newHeight<viewport()->height()){
        newHeight=viewport()->height();
        pps=qreal( newHeight )/secs;
    }
*/
    m_pixelPerSecond=pps;

    emit pixelPerSecondChanged(pps);

    m_gatherLabel->setFixedHeight(newHeight);

    qreal factor=pps / oldPPS;        // !!!! XXXX same as for traces

    adjustScrollBar( verticalScrollBar(), factor);

    m_leftRuler->update();
}

void GatherView::setPixelPerUnits( qreal pixPerTrace, qreal pixPerSecond){

    setUpdatesEnabled( false );

    setPixelPerTrace(pixPerTrace);

    setPixelPerSecond(pixPerSecond);

    setUpdatesEnabled( true );

    updateGeometry();

}


void GatherView::setFixedScale(bool on){

    if( on==m_fixedScale) return;

    m_fixedScale=on;

    emit fixedScaleChanged(on);
}

void GatherView::normalize(){

    if( !m_gather || m_gather->empty()) return;

    qreal ppt=qreal(viewport()->width())/m_gather->size();
    qreal pps=qreal(viewport()->height())/seconds();
    setPixelPerUnits(ppt,pps);

    updateGeometry();
    m_topRuler->update();
    m_leftRuler->update();
}


void GatherView::zoom( QRect rect ){

    if( m_fixedScale ) return;

    const int ZOOM_PIXEL_LIMIT=6;           // selection of less pixels is considered zoom out

    setUpdatesEnabled(false);

    int newWidth=(rect.width()>ZOOM_PIXEL_LIMIT)?
                m_gatherLabel->width() * viewport()->width() / rect.width() :
                viewport()->width();

    int newHeight=(rect.height()>ZOOM_PIXEL_LIMIT)?
                m_gatherLabel->height() * viewport()->height() / rect.height() :
                viewport()->height();
    qreal oldPixelPerTrace=m_pixelPerTrace;
    qreal oldPixelPerSecond=m_pixelPerSecond;
    qreal newPixelPerTrace=(m_gather && m_gather->size()>0) ? qreal(newWidth)/m_gather->size():qreal(newWidth);
    qreal newPixelPerSecond=(m_gather && m_gather->size()>0) ? qreal(newHeight)/seconds() : qreal(newHeight);

    m_gatherLabel->setFixedSize(newWidth, newHeight);
    horizontalScrollBar()->setRange(0, newWidth);
    verticalScrollBar()->setRange(0,newHeight);


    m_pixelPerTrace=newPixelPerTrace;
    m_pixelPerSecond=newPixelPerSecond;

    emit pixelPerTraceChanged(m_pixelPerTrace);
    emit pixelPerSecondChanged(m_pixelPerSecond);

    int newX=rect.left()*newPixelPerTrace/oldPixelPerTrace;
    if( newX>=newWidth-viewport()->width()){
        newX=newWidth-viewport()->width();
    }
    if( newX<0) newX=0;

    int newY=rect.top()*newPixelPerSecond/oldPixelPerSecond;
    if( newY>=newHeight-viewport()->height()){
        newY=newHeight-viewport()->height();
    }
    if( newY<0 ) newY=0;

    horizontalScrollBar()->setRange(0,m_gatherLabel->width()-viewport()->width());
    verticalScrollBar()->setRange(0,m_gatherLabel->height()-viewport()->height());

    horizontalScrollBar()->setValue(newX);
    verticalScrollBar()->setValue(newY);

    setUpdatesEnabled(true);

    update();
}



void GatherView::zoomBy( qreal factor ){


    if( m_fixedScale ) return;

    // adjust factor such that gather at least fills the viewport
    qreal minPPT=qreal(viewport()->width())/m_gather->size();   // minimum pixel per trace
    qreal minPPS=qreal(viewport()->height())/(lt()-ft());       // minium pixel per second
    if( factor*m_pixelPerTrace < minPPT ) factor=minPPT/m_pixelPerTrace;
    if( factor*m_pixelPerSecond < minPPS ) factor=minPPS/m_pixelPerSecond;

    setPixelPerUnits(factor*m_pixelPerTrace, factor*m_pixelPerSecond);
    //adjustScrollBar( horizontalScrollBar(), factor);
    //adjustScrollBar( verticalScrollBar(), factor);

/*
    QSize oldSize=m_gatherLabel->size();
    QSize newSize=factor * m_gatherLabel->size();
    if( newSize.width()<viewport()->width()) newSize.setWidth(viewport()->width());
    if( newSize.height()<viewport()->height()) newSize.setHeight( viewport()->height());

    m_gatherLabel->setFixedSize( newSize );
    adjustScrollBar( horizontalScrollBar(), qreal(newSize.width())/oldSize.width());// factor);
    adjustScrollBar( verticalScrollBar(), qreal(newSize.height())/oldSize.height());//factor);
*/
    update();
}

void GatherView::updateLayout(){

    m_leftRuler->setGeometry( 0, viewport()->y(), RULER_WIDTH, viewport()->height() );
    m_topRuler->setGeometry( viewport()->x(), 0, viewport()->width(), RULER_HEIGHT);
    m_axxisLabelWidget->setGeometry(0, 0, viewport()->x(), viewport()->y());
}


void GatherView::updateTimeRange(){

    qreal ft=std::numeric_limits<qreal>::max();
    qreal lt=std::numeric_limits<qreal>::lowest();

    if( m_gather ){

        const seismic::Gather& gather=*m_gather;
        for( const seismic::Trace& trc : gather ){

            if( trc.ft()<ft ){
                ft=trc.ft();
            }

            if( trc.lt()>lt ){
                lt=trc.lt();
            }

        }
    }

    m_ft=ft;
    m_lt=lt;
}

void GatherView::resizeEvent(QResizeEvent *ev){
//

    if( !m_fixedScale){
        int oldlblw=m_gatherLabel->width();
        int oldWidth=ev->oldSize().width();
        int oldHeight=ev->oldSize().height();
        int oldX=horizontalScrollBar()->value();
        int oldY=verticalScrollBar()->value();
        zoom( QRect(oldX, oldY, oldWidth, oldHeight));
    }
    else{

        horizontalScrollBar()->setPageStep(viewport()->width());
        verticalScrollBar()->setPageStep(viewport()->height());
        horizontalScrollBar()->setRange(0,m_gatherLabel->width()-viewport()->width());
        verticalScrollBar()->setRange(0,m_gatherLabel->height()-viewport()->height());
        m_gatherLabel->update();
        m_leftRuler->update();
        m_topRuler->update();

    }
 //
   // std::cout<<"w="<<gatherLabel()->width()<<std::endl;
    updateLayout();

   // update();
}


bool GatherView::eventFilterExplore(QWidget* widget, QMouseEvent *mouseEvent){

    static bool drag=false;
    static QPoint last;

    if( mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->button()==Qt::LeftButton ){
        last=mouseEvent->pos();
        drag=true;
        m_gatherLabel->setCursor(QCursor(Qt::DragMoveCursor) );
    }
    else if( mouseEvent->type()==QEvent::MouseMove && drag ){

        QPoint delta = last - mouseEvent->pos();
        horizontalScrollBar()->setValue( horizontalScrollBar()->value() + delta.x() );
        verticalScrollBar()->setValue( verticalScrollBar()->value() + delta.y() );
        last = mouseEvent->pos();
    }
    else if( mouseEvent->type()==QEvent::MouseButtonRelease && drag ){
        drag=false;
        m_gatherLabel->setCursor(modeCursor(m_mouseMode));  // restore previous cursor
    }

    return true;
}


bool GatherView::eventFilterZoom(QWidget* widget, QMouseEvent *mouseEvent){

    if( mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->button()==Qt::LeftButton ){

            mouseSelection=true;
            mouseSelectionWidget=widget;
            mouseSelectionStart=mouseEventToLabel(mouseEvent->pos(), true);
            if( !rubberBand ){
                rubberBand=new QRubberBand( QRubberBand::Shape::Rectangle, m_gatherLabel);
            }
            rubberBand->setGeometry(QRect(mouseSelectionStart, QSize()));
            rubberBand->show();

            // set the appropriate cursor even if we came here using CTRL key, i.s. mouseMode is not zoom
            m_gatherLabel->setCursor(modeCursor(MouseMode::Zoom) );
    }
    else if( mouseEvent->type()==QEvent::MouseMove && mouseSelection ){

        if( widget==mouseSelectionWidget ){

            QPoint cur=mouseEventToLabel(mouseEvent->pos(), false);

            rubberBand->setGeometry(QRect(mouseSelectionStart, cur).normalized());
        }

    }
    else if( mouseEvent->type()==QEvent::MouseButtonRelease && mouseEvent->button()==Qt::LeftButton){

        mouseSelection=false;
       // mouseSelectionWidget=nullptr;
        QPoint end=mouseEventToLabel(mouseEvent->pos(), false);
        rubberBand->hide();

        zoom( QRect(mouseSelectionStart, end).normalized());

        m_gatherLabel->setCursor(modeCursor(m_mouseMode));  // restore previous cursor
    }

    return true;
}

bool GatherView::eventFilterSelect(QWidget* widget, QMouseEvent *mouseEvent){

    // select point on double click
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseButtonDblClick){
        QPoint p=mouseEvent->pos();
        int trace=p.x()/m_pixelPerTrace;
        qreal secs=m_ft + p.y()/m_pixelPerSecond;

        //if( trace>=0 ) emit traceSelected(static_cast<size_t>(trace));
        if( trace>=0 ){
            int iline=(*m_gather)[trace].header().at("iline").intValue();
            int xline=(*m_gather)[trace].header().at("xline").intValue();
            emit pointSelected( SelectionPoint( iline, xline, secs ));
        }
    }
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseButtonRelease ){

        int trace=mouseEvent->pos().x() / m_pixelPerTrace;

        emit traceClicked(trace);
    }
    else if( widget==m_topRuler && mouseEvent->type()==QEvent::MouseButtonDblClick ){

        QPoint p=mouseEvent->pos();
        int trace=std::floor(qreal(horizontalScrollBar()->value() + p.x())/m_pixelPerTrace);
        emit topRulerClicked(trace);
    }
    else if( widget==m_leftRuler && mouseEvent->type()==QEvent::MouseButtonDblClick ){

        QPoint p=mouseEvent->pos();
        qreal secs=m_ft + qreal(verticalScrollBar()->value() + p.y() )/m_pixelPerSecond;
        emit leftRulerClicked(secs);
    }

    return true;
}

bool GatherView::eventFilterPick(QWidget* widget, QMouseEvent *mouseEvent){

    // pick
    if( widget!=m_gatherLabel ) return false;

    if( (QEvent::MouseButtonPress && mouseEvent->buttons()&Qt::LeftButton) ||
            (mouseEvent->type()==QEvent::MouseMove &&
             mouseEvent->buttons()&Qt::LeftButton &&
             m_picker->mode()==PickMode::Single) ){
        QPoint p=mouseEvent->pos();
        int trace=static_cast<int>(p.x()/m_pixelPerTrace);
        qreal secs=m_ft + p.y()/m_pixelPerSecond;
        m_picker->pick(trace, secs);
    }

    // short cut for delete picks in pick mode
    if( (QEvent::MouseButtonPress && mouseEvent->buttons()&Qt::MiddleButton) ||
            (mouseEvent->type()==QEvent::MouseMove &&
             mouseEvent->buttons()&Qt::MiddleButton) ){
        QPoint p=mouseEvent->pos();
        int trace=static_cast<int>(p.x()/m_pixelPerTrace);
        m_picker->deleteSingle(trace);  // don't use pichmode
    }

    return true;
}

bool GatherView::eventFilterDeletePick(QWidget* widget, QMouseEvent *mouseEvent){

    if( widget!=m_gatherLabel ) return false;

    if(mouseEvent->type()==QEvent::MouseButtonPress ||
            (mouseEvent->type()==QEvent::MouseMove &&
             mouseEvent->buttons()&Qt::LeftButton &&
             m_picker->mode()==PickMode::Single) ){

        QPoint p=mouseEvent->pos();
        int trace=static_cast<int>(p.x()/m_pixelPerTrace);
        qreal secs=m_ft + p.y()/m_pixelPerSecond;
        m_picker->deletePick(trace);
    }

    return true;
}



bool GatherView::eventFilter(QObject *obj, QEvent *ev){

    QWidget* widget=dynamic_cast<QWidget*>(obj);
    QMouseEvent* mouseEvent=dynamic_cast<QMouseEvent*>(ev);

    if( !widget || !mouseEvent ) return QObject::eventFilter(obj,ev);

    if( !(widget==m_gatherLabel || widget==m_leftRuler || widget==m_topRuler)) return QObject::eventFilter(obj,ev);

    // emit position move on label
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseMove ){

        QPoint p=mouseEvent->pos();
        int trace=p.x()/m_pixelPerTrace;
        qreal secs=m_ft + p.y()/m_pixelPerSecond;
        emit mouseOver(trace, secs);
    }
    // emit position, mouse move on top tuler
    else if( widget==m_topRuler && mouseEvent->type()==QEvent::MouseMove ){
        QPoint p=mouseEvent->pos();
        int trace=std::floor(qreal(horizontalScrollBar()->value() + p.x())/m_pixelPerTrace);
        qreal secs=m_ft;
        emit mouseOver(trace, secs);
    }
    // emit position, mouse move on left ruler
    else if( widget==m_leftRuler && mouseEvent->type()==QEvent::MouseMove ){

        QPoint p=mouseEvent->pos();
        int trace=0;    // rightmost
        qreal secs=m_ft + qreal(verticalScrollBar()->value() + p.y() )/m_pixelPerSecond;
        emit mouseOver(trace, secs);
    }


    if( mouseEvent->modifiers()==Qt::ControlModifier ){
        return eventFilterZoom( widget, mouseEvent);
    }

    if( mouseEvent->modifiers()==Qt::ShiftModifier || mouseEvent->type()==QEvent::MouseButtonDblClick ){
        return eventFilterSelect( widget, mouseEvent);
    }

    if( m_mouseMode==MouseMode::Explore ){
        return eventFilterExplore( widget, mouseEvent );
    }

    if( m_mouseMode == MouseMode::Zoom){
        return eventFilterZoom( widget, mouseEvent);
    }

    if( m_mouseMode == MouseMode::Select ){
        return eventFilterSelect( widget, mouseEvent);
    }

    if( m_mouseMode == MouseMode::Pick ){
        return eventFilterPick(widget, mouseEvent);
    }

    if( m_mouseMode == MouseMode::DeletePick ){
        return eventFilterDeletePick(widget, mouseEvent);
    }

    return QObject::eventFilter(obj,ev);

}


/* ORIGINAL
bool GatherView::eventFilter(QObject *obj, QEvent *ev){

    QWidget* widget=dynamic_cast<QWidget*>(obj);
    QMouseEvent* mouseEvent=dynamic_cast<QMouseEvent*>(ev);

    if( !widget || !mouseEvent ) return QObject::eventFilter(obj,ev);

    if( !(widget==m_gatherLabel || widget==m_leftRuler || widget==m_topRuler)) return QObject::eventFilter(obj,ev);

    // select point on double click
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseButtonDblClick){
        QPoint p=mouseEvent->pos();
        int trace=p.x()/m_pixelPerTrace;
        qreal secs=m_ft + p.y()/m_pixelPerSecond;

        //if( trace>=0 ) emit traceSelected(static_cast<size_t>(trace));
        if( trace>=0 ){
            int iline=(*m_gather)[trace].header().at("iline").intValue();
            int xline=(*m_gather)[trace].header().at("xline").intValue();
            emit pointSelected( SelectionPoint( iline, xline, secs ));
        }
    }

    // emit position
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseMove ){

        QPoint p=mouseEvent->pos();
        int trace=p.x()/m_pixelPerTrace;
        qreal secs=m_ft + p.y()/m_pixelPerSecond;
        emit mouseOver(trace, secs);
    }

    // pick
    if( m_mouseMode == MouseMode::Pick && widget==m_gatherLabel ){

        if(mouseEvent->type()==QEvent::MouseButtonPress ||
                (mouseEvent->type()==QEvent::MouseMove &&
                 mouseEvent->buttons()&Qt::LeftButton &&
                 m_picker->mode()==PickMode::Single) ){
            QPoint p=mouseEvent->pos();
            int trace=static_cast<int>(p.x()/m_pixelPerTrace);
            qreal secs=m_ft + p.y()/m_pixelPerSecond;
            m_picker->pick(trace, secs);
            return true;
        }
    }

    // delete pick
    if( m_mouseMode == MouseMode::DeletePick && widget==m_gatherLabel ){

        if(mouseEvent->type()==QEvent::MouseButtonPress ||
                (mouseEvent->type()==QEvent::MouseMove &&
                 mouseEvent->buttons()&Qt::LeftButton &&
                 m_picker->mode()==PickMode::Single) ){

            QPoint p=mouseEvent->pos();
            int trace=static_cast<int>(p.x()/m_pixelPerTrace);
            qreal secs=m_ft + p.y()/m_pixelPerSecond;
            m_picker->deletePick(trace);
            return true;
        }
    }

    // trace header
    if( widget==m_gatherLabel && mouseEvent->type()==QEvent::MouseButtonRelease &&
            (mouseEvent->modifiers()==Qt::ShiftModifier || m_mouseMode==MouseMode::Select) ){

        int trace=mouseEvent->pos().x() / m_pixelPerTrace;

        emit traceClicked(trace);

        return true;
    }

    // emit position, mouse move on top tuler
    if( widget==m_topRuler && mouseEvent->type()==QEvent::MouseMove ){

        QPoint p=mouseEvent->pos();
        int trace=std::floor(qreal(horizontalScrollBar()->value() + p.x())/m_pixelPerTrace);
        qreal secs=m_ft;
        emit mouseOver(trace, secs);
    }

    // click on top ruler, emit trace
    if( widget==m_topRuler && mouseEvent->type()==QEvent::MouseButtonDblClick ){

        QPoint p=mouseEvent->pos();
        int trace=std::floor(qreal(horizontalScrollBar()->value() + p.x())/m_pixelPerTrace);
        emit topRulerClicked(trace);
    }

    // emit position, mouse move on left ruler
    if( widget==m_leftRuler && mouseEvent->type()==QEvent::MouseMove ){

        QPoint p=mouseEvent->pos();
        int trace=0;    // rightmost
        qreal secs=m_ft + qreal(verticalScrollBar()->value() + p.y() )/m_pixelPerSecond;
        emit mouseOver(trace, secs);
    }

    // click on left ruler, emit time
    if( widget==m_leftRuler && mouseEvent->type()==QEvent::MouseButtonDblClick ){

        QPoint p=mouseEvent->pos();
        qreal secs=m_ft + qreal(verticalScrollBar()->value() + p.y() )/m_pixelPerSecond;
        emit leftRulerClicked(secs);
    }

    if (mouseEvent->modifiers() != Qt::ControlModifier && m_mouseMode!=MouseMode::Zoom &&
            !mouseSelection) return QObject::eventFilter(obj, ev);

    // zoom

    if( mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->button()==Qt::LeftButton ){

            mouseSelection=true;
            mouseSelectionWidget=widget;
            mouseSelectionStart=mouseEventToLabel(mouseEvent->pos(), true);
            if( !rubberBand ){
                rubberBand=new QRubberBand( QRubberBand::Shape::Rectangle, m_gatherLabel);
            }
            rubberBand->setGeometry(QRect(mouseSelectionStart, QSize()));
            rubberBand->show();
    }
    else if( mouseEvent->type()==QEvent::MouseMove && mouseSelection ){

        if( widget==mouseSelectionWidget ){

            QPoint cur=mouseEventToLabel(mouseEvent->pos(), false);

            rubberBand->setGeometry(QRect(mouseSelectionStart, cur).normalized());
        }

    }
    else if( mouseEvent->type()==QEvent::MouseButtonRelease && mouseEvent->button()==Qt::LeftButton){

        mouseSelection=false;
       // mouseSelectionWidget=nullptr;
        QPoint end=mouseEventToLabel(mouseEvent->pos(), false);
        rubberBand->hide();

        zoom( QRect(mouseSelectionStart, end).normalized());

    }
    else{
        return QObject::eventFilter(obj,ev);
    }

    return true;
}
*/



QPoint GatherView::mouseEventToLabel( QPoint pt, bool start ){

    if( mouseSelectionWidget==m_leftRuler ){
        if( start ){
            pt.setX(horizontalScrollBar()->value());
        }
        else{
            pt.setX(horizontalScrollBar()->value()+horizontalScrollBar()->pageStep());
        }
        pt.setY(verticalScrollBar()->value() + pt.y() );
    }
    else if( mouseSelectionWidget==m_topRuler){
        pt.setX( horizontalScrollBar()->value() + pt.x() );
        if( start ){
           pt.setY( verticalScrollBar()->value() );
        }
        else{
             pt.setY( verticalScrollBar()->value() + verticalScrollBar()->pageStep() );
        }
    }

    if( pt.x()<0 ) pt.setX(0);
    if( pt.x()>m_gatherLabel->width()-1) pt.setX( m_gatherLabel->width() -1);
    if( pt.y() < 0 ) pt.setY(0);
    if( pt.y() > m_gatherLabel->height() - 1) pt.setY( m_gatherLabel->height() -1);

    return pt;
}

void GatherView::adjustScrollBar(QScrollBar *scrollBar, qreal factor)
{

    int newValue=int(factor * scrollBar->value()
                     + ((factor - 1) * scrollBar->pageStep()/2));
    scrollBar->setValue(newValue);
}
