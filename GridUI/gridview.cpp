#include "gridview.h"

#include<QScrollBar>
#include<QPainter>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QApplication>
#include<isolinecomputer.h>
#include<gridviewer.h>

#include<iostream>
#include<cmath>



GridView::GridView(QWidget* parent):QScrollArea(parent),
    m_colorTable(new ColorTable(this))
{

    m_label = new ViewLabel( this );
    m_label->setBackgroundRole(QPalette::Base);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    m_nullColor=m_label->palette().window().color();

    setWidget( m_label);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    setViewportMargins( RULER_WIDTH, RULER_HEIGHT, 0, 0);

    m_leftRuler=new Ruler( this, Ruler::VERTICAL_RULER);
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), m_leftRuler, SLOT( update()) );
    //connect( this, SIGNAL(gridChanged(std::shared_ptr<Grid2D>)), m_leftRuler, SLOT(update()));

    m_topRuler=new Ruler( this, Ruler::HORIZONTAL_RULER);
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), m_topRuler, SLOT( update()) );
    //connect( this, SIGNAL(gridChanged(std::shared_ptr<Grid2D>)), m_topRuler, SLOT(update()));

    m_label->setMouseTracking(true);
    setMouseTracking(true);

    connect( this, SIGNAL(gridToImageTransformChanged(QTransform)), m_label, SLOT(updateLineSegments()));
    connect( this, SIGNAL(gridToImageTransformChanged(QTransform)), m_leftRuler, SLOT(update()));
    connect( this, SIGNAL(gridToImageTransformChanged(QTransform)), m_topRuler, SLOT(update()));

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(onColorTableChanged()));
    connect( m_colorTable, SIGNAL(rangeChanged( std::pair<double,double>  )), this, SLOT(onColorTableChanged()));
    connect( m_colorTable, SIGNAL(powerChanged( double )), this, SLOT(onColorTableChanged()));

    connect( this, SIGNAL(inlineOrientationChanged(AxxisOrientation)), m_leftRuler, SLOT(update()));
    connect( this, SIGNAL(inlineOrientationChanged(AxxisOrientation)), m_topRuler, SLOT(update()));

    connect( this, SIGNAL(polylineSelected(QVector<QPoint>)), m_label, SLOT(update()));

     updateGeometry();

    m_label->installEventFilter(this);
    m_leftRuler->installEventFilter(this);
    m_topRuler->installEventFilter(this);
}

GridView::~GridView(){
}

void GridView::setGrid( std::shared_ptr<Grid2D<float> > g){
    //if( g==m_grid) return;
    m_grid=g;
    emit(gridChanged(g));

    m_label->setIsoLineSegments( QVector<QLineF>());        // isolines not valid for new grid

    m_label->setImage( grid2image());

    setGridToImageTransform( computeGridToImageTransform());
}

void GridView::setHighlightedCDPs( QVector<SelectionPoint> inlinesAndCrosslines){

    //std::cout<<"GridView::setHighlightedCDPS n="<<inlinesAndCrosslines.size()<<std::endl;
    if( m_highlightedCDPs==inlinesAndCrosslines) return;

    m_highlightedCDPs=inlinesAndCrosslines;

    //std::cout<<"GridView::setHighlightedCDPS n="<<inlinesAndCrosslines.size()<<std::endl;

    m_label->update();
}

void GridView::setColorMapping( const std::pair<double,double>& m){
    Q_ASSERT( m_colorTable);
    m_colorTable->setRange(m);
}

void GridView::setColors(const QVector<QRgb>& c){

    Q_ASSERT( m_colorTable);
    m_colorTable->setColors(c);
}

void GridView::setNullColor( QColor col ){

    if( col==m_nullColor ) return;

    m_nullColor=col;

    emit nullColorChanged(col);

    m_label->setImage( grid2image() );
}


void GridView::setIsoLineValues( const QVector<double>& isovals){

    if( m_isoLineValues==isovals) return;

    QVector<QLineF> segs;

    if( isovals.empty()){   // need special case because dont want to mess with progressdialog
        m_isoLineValues=isovals;
        label()->setIsoLineSegments(segs);
        return;
    }

    QProgressDialog progress(QString("Computing contours"), QString("Cancel"), 0, isovals.size() );
    progress.show();
    progress.setCancelButton(0);
    qApp->processEvents();
    //progress.setWindowTitle("Computing contours");



    for( int i=0; i<isovals.size(); i++ ){

        segs.append(IsoLineComputer::compute(*m_grid, isovals[i]) );
        progress.setValue(i);
        qApp->processEvents();
    }


    m_isoLineValues=isovals;
    label()->setIsoLineSegments(segs);

}


void GridView::setInlineOrientation( AxxisOrientation o){

    if( o==m_inlineOrientation) return;

    m_inlineOrientation=o;

    emit inlineOrientationChanged(o);

    setGridToImageTransform( computeGridToImageTransform());

    m_label->setImage( grid2image());

}

void GridView::setInlineDirection( AxxisDirection d ){

    if( d==m_inlineDirection) return;

    m_inlineDirection=d;

    emit inlineDirectionChanged( d );

    setGridToImageTransform( computeGridToImageTransform());

    m_label->setImage( grid2image());

}

void GridView::setCrosslineDirection( AxxisDirection d ){

    if( d==m_crosslineDirection) return;

    m_crosslineDirection=d;

    emit crosslineDirectionChanged( d );

    setGridToImageTransform( computeGridToImageTransform());

   m_label->setImage( grid2image());
}

// set all 3 params at once with only 1 update
void GridView::setOrientationParams( AxxisOrientation ilOrientation, AxxisDirection ilDirection, AxxisDirection xlDirection){

    if( ilOrientation==m_inlineOrientation && ilDirection==m_inlineDirection && xlDirection==m_crosslineDirection) return;

    m_inlineOrientation=ilOrientation;
    m_inlineDirection=ilDirection;
    m_crosslineDirection=xlDirection;

    setGridToImageTransform( computeGridToImageTransform());

    m_label->setImage( grid2image());
}

void GridView::setInlineAxxisLabel( const QString& s){

    if( s==m_inlineAxxisLabel) return;

    m_inlineAxxisLabel=s;

    emit inlineAxxisLabelChanged(s);
}

void GridView::setCrosslineAxxisLabel( const QString& s ){

    if( s==m_crosslineAxxisLabel) return;

    m_crosslineAxxisLabel=s;

    emit crosslineAxxisLabelChanged(s);
}

void GridView::onColorTableChanged(){

    m_label->setImage( grid2image());
}

void GridView::zoomBy( qreal factor ){

    //setUpdatesEnabled(false);

    QSize newSize=factor * m_label->size();
    if( newSize.width()<viewport()->width()) newSize.setWidth(viewport()->width());
    if( newSize.height()<viewport()->height()) newSize.setHeight( viewport()->height());

    m_label->setFixedSize( newSize );
    adjustScrollBar( horizontalScrollBar(), factor);
    adjustScrollBar( verticalScrollBar(), factor);

    //m_leftRuler->update();
    //m_topRuler->update();

    //setUpdatesEnabled(true);

    update();
}

void GridView::zoomFit(){

    setUpdatesEnabled(false);

    m_label->setFixedSize( horizontalScrollBar()->pageStep(), verticalScrollBar()->pageStep());
    horizontalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(0);

    //m_leftRuler->update();
    //m_topRuler->update();

    setUpdatesEnabled(true);
    update();
}


void GridView::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}


void GridView::setGridToImageTransform( const QTransform& t){

    if( t==m_gridToImageTransform) return;

    m_gridToImageTransform = t;

    emit gridToImageTransformChanged( t );

    m_imageToGridTransform = m_gridToImageTransform.inverted();

    update();
}


void GridView::updateLayout(){

    setUpdatesEnabled( false );

    m_leftRuler->setGeometry( 0, viewport()->y(), RULER_WIDTH, viewport()->height() );
    m_topRuler->setGeometry( viewport()->x(), 0, viewport()->width(), RULER_HEIGHT );

    setUpdatesEnabled( true );
}

void GridView::resizeEvent(QResizeEvent *){

    m_label->setMinimumSize( viewport()->size());

    m_topRuler->update();
    m_leftRuler->update();

    updateLayout();

}


QPoint GridView::mouseEventToLabel( QPoint pt, bool start ){

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
    if( pt.x()>m_label->width()-1) pt.setX( m_label->width() -1);
    if( pt.y() < 0 ) pt.setY(0);
    if( pt.y() > m_label->height() - 1) pt.setY( m_label->height() -1);

    return pt;
}

void GridView::zoom( QRect rect ){


    const int ZOOM_PIXEL_LIMIT=6;           // selection of less pixels is considered zoom out

    setUpdatesEnabled(false);

    int oldWidth=m_label->width();
    int oldHeight=m_label->height();

    int newWidth=(rect.width()>ZOOM_PIXEL_LIMIT)?
                m_label->width() * viewport()->width() / rect.width() :
                viewport()->width();

    int newHeight=(rect.height()>ZOOM_PIXEL_LIMIT)?
                m_label->height() * viewport()->height() / rect.height() :
                viewport()->height();

    m_label->setFixedSize(newWidth, newHeight);
    horizontalScrollBar()->setRange(0, newWidth);
    verticalScrollBar()->setRange(0,newHeight);

    int newX=rect.left()*newWidth/oldWidth;
    if( newX>=newWidth-viewport()->width()){
        newX=newWidth-viewport()->width();
    }
    if( newX<0) newX=0;

    int newY=rect.top()*newHeight/oldHeight;
    if( newY>=newHeight-viewport()->height()){
        newY=newHeight-viewport()->height();
    }
    if( newY<0 ) newY=0;

    horizontalScrollBar()->setValue(newX);
    verticalScrollBar()->setValue(newY);

    setUpdatesEnabled(true);

    update();
}


bool GridView::eventFilter(QObject *obj, QEvent *ev){

    QWidget* widget=dynamic_cast<QWidget*>(obj);
    QMouseEvent* mouseEvent=dynamic_cast<QMouseEvent*>(ev);

    if( !widget || !mouseEvent ) return QObject::eventFilter(obj,ev);

    if( !(widget==m_label || widget==m_leftRuler || widget==m_topRuler)) return QObject::eventFilter(obj,ev);

    // dispatch point
    if (widget==m_label && mouseEvent->type()==QEvent::MouseButtonDblClick){
        int x=mouseEvent->x();
        int y=mouseEvent->y();
        QPointF p=imageToGridTransform().map( QPointF(x,y));
        int iline=std::round( p.y() );
        int xline=std::round( p.x() );
        emit(pointSelected(QPoint(iline,xline)));
    }

    // select line add point
    if(widget==m_label && mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->modifiers()==Qt::ShiftModifier){
        int x=mouseEvent->x();
        int y=mouseEvent->y();
        QPointF p=imageToGridTransform().map( QPointF(x,y));
        int iline=std::round( p.y() );
        int xline=std::round( p.x() );
        QPoint point(iline,xline);
        m_polyline.append(point);
        emit polylineSelected(m_polyline);  // triggers update of label
    }

    if (mouseEvent->modifiers() != Qt::ControlModifier && !mouseSelection) return QObject::eventFilter(obj, ev);

    // zoom

    if( mouseEvent->type()==QEvent::MouseButtonPress && mouseEvent->button()==Qt::LeftButton ){

            mouseSelection=true;
            mouseSelectionWidget=widget;
            mouseSelectionStart=mouseEventToLabel(mouseEvent->pos(), true);
            if( !rubberBand ){
                rubberBand=new QRubberBand( QRubberBand::Shape::Rectangle, m_label);
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


void GridView::keyPressEvent(QKeyEvent * event){

    if( event->key()==Qt::Key_Escape ){
        m_polyline.clear();
        emit polylineSelected(m_polyline);  // triggers update of label
    }
}


QImage GridView::grid2image(){

    if( !m_grid) return QImage();

    Q_ASSERT( m_colorTable);

    int n_colors=m_colorTable->colors().size();
    Q_ASSERT( n_colors>0);

    if( m_inlineOrientation == AxxisOrientation::Horizontal ){

        QImage img( m_grid->bounds().width(), m_grid->bounds().height(), QImage::Format_ARGB32);

        int y0img=(m_inlineDirection==AxxisDirection::Ascending)? 0 : img.height()-1;
        int dyimg=(m_inlineDirection==AxxisDirection::Ascending)?1:-1;

        int x0img=(m_crosslineDirection==AxxisDirection::Ascending)? 0 : img.width()-1;
        int dximg=(m_crosslineDirection==AxxisDirection::Ascending)? 1 : -1;


        int yimg=y0img;
        for( Grid2D<float>::index_type i=m_grid->bounds().i1(); i<=m_grid->bounds().i2(); i++, yimg+=dyimg){

            int ximg=x0img;
            for(
                Grid2D<float>::index_type j=m_grid->bounds().j1(); j<=m_grid->bounds().j2(); j++, ximg+=dximg){

                Grid2D<float>::value_type v=(*m_grid)( i, j);
                QRgb col;

                if( v==Grid2D<float>::NULL_VALUE){
                    col=m_nullColor.rgb();
                }
                else{
                    col=m_colorTable->map(v);
                    /*
                    int idx=std::round(n_colors*(v-first)/(last-first));
                    if(idx<0) idx=0;
                    if(idx>=n_colors) idx=n_colors-1;
                    col=m_colorTable->colors()[idx];
                    */
                }
                //img.setPixel( i-m_grid->bounds().i1(), j-m_grid->bounds().j1(), col);//static_cast<int>((COLORTABLE_SIZE-1)*v) );
                img.setPixel( ximg, yimg,  col);//
            }
        }

        return img;
    }

    else  if( m_inlineOrientation == AxxisOrientation::Vertical ){                    //  i -> x, j -> y

        QImage img( m_grid->bounds().height(), m_grid->bounds().width(), QImage::Format_ARGB32);

        int y0img=(m_crosslineDirection==AxxisDirection::Ascending)? 0 : img.height()-1;
        int dyimg=(m_crosslineDirection==AxxisDirection::Ascending)?1:-1;

        int x0img=(m_inlineDirection==AxxisDirection::Ascending)? 0 : img.width()-1;
        int dximg=(m_inlineDirection==AxxisDirection::Ascending)? 1 : -1;


        int ximg=x0img;
        for( Grid2D<float>::index_type i=m_grid->bounds().i1(); i<=m_grid->bounds().i2(); i++, ximg+=dximg){

            int yimg=y0img;
            for( Grid2D<float>::index_type j=m_grid->bounds().j1(); j<=m_grid->bounds().j2(); j++, yimg+=dyimg){

                Grid2D<float>::value_type v=(*m_grid)( i, j);
                QRgb col;

                if( v==Grid2D<float>::NULL_VALUE){
                    col=m_nullColor.rgb();
                }
                else{
                    col=m_colorTable->map(v);
                    /*
                    int idx=std::round(n_colors*(v-first)/(last-first));
                    if(idx<0) idx=0;
                    if(idx>=n_colors) idx=n_colors-1;
                    col=m_colorTable->colors()[idx];
                    */
                }
                //img.setPixel( i-m_grid->bounds().i1(), j-m_grid->bounds().j1(), col);//static_cast<int>((COLORTABLE_SIZE-1)*v) );
                img.setPixel( ximg, yimg,  col);//
            }

        }

        return img;
    }
    else{
        qFatal("There was a impossible inline orientation!");
    }
}

QTransform GridView::computeGridToImageTransform()const{

    if( inlineOrientation() == AxxisOrientation::Horizontal ){
        return computeGrid2ImageTransform_IlHorizontal();
    }
    else{
        return computeGrid2ImageTransform_IlVertical();
    }
}


QTransform GridView::computeGrid2ImageTransform_IlHorizontal()const{

    if( !m_grid) return QTransform();

    bool il_ascending = ( inlineDirection()==AxxisDirection::Ascending);
    bool xl_ascending = ( crosslineDirection()==AxxisDirection::Ascending);

    Grid2D<float>::bounds_type bounds=m_grid->bounds();
    int il1=bounds.i1();
    int il2=bounds.i2();
    int xl1=bounds.j1();
    int xl2=bounds.j2();

    int img_width = m_label->width();
    int img_height = m_label->height();

    double m11=1;
    double m22=1;
    double dx=0;
    double dy=0;

    if( xl_ascending){

        m11 = double( img_width - 1 ) / ( xl2 - xl1 );
        dx = - m11 * xl1;
    }
    else{

        m11 = double( img_width - 1 ) / ( xl1 - xl2 );
        dx  = -m11 * xl2;

    }

    if( il_ascending ){

        m22 = double( img_height - 1 ) / ( il2 - il1 );
        dy = - m22 * il1;
    }
    else{

        m22 = double( img_height - 1 ) / ( il1 - il2 );
        dy  = -m22 * il2;
    }


    return QTransform( m11, 0, 0, 0, m22, 0, dx, dy, 1);
}


QTransform GridView::computeGrid2ImageTransform_IlVertical()const{

    if( !m_grid) return QTransform();

    bool il_ascending = ( inlineDirection()==AxxisDirection::Ascending);
    bool xl_ascending = ( crosslineDirection()==AxxisDirection::Ascending);

    Grid2D<float>::bounds_type bounds=m_grid->bounds();
    int il1=bounds.i1();
    int il2=bounds.i2();
    int xl1=bounds.j1();
    int xl2=bounds.j2();

    int img_width = m_label->width();
    int img_height = m_label->height();

    double m12=0;
    double m21=0;
    double dx=0;
    double dy=0;

    if( xl_ascending){

        m12 = double( img_height - 1 ) / ( xl2 - xl1 );
        dy = - m12 * xl1;
    }
    else{

        m12 = double( img_height - 1 ) / ( xl1 - xl2 );
        dy = - m12 * xl2;

    }

    if( il_ascending ){

        m21 = double( img_width - 1 ) / ( il2 - il1 );
        dx = - m21 * il1;
    }
    else{

        m21 = double( img_width - 1 ) / ( il1 - il2 );
        dx = - m21 * il2;
    }


    return QTransform( 0, m12, 0, m21, 0, 0, dx, dy, 1);
}





ViewLabel::ViewLabel( GridView* parent) : QLabel(parent), m_view(parent),
    m_transformationMode(Qt::FastTransformation), m_isoLineColor(Qt::black), m_isoLineWidth(1){
    Q_ASSERT(m_view);
}


void ViewLabel::setIsoLineSegments(const QVector<QLineF> & segs){

    m_isoLineSegments=segs;

    emit isoLineSegmentsChanged(m_isoLineSegments);


    updateLineSegments();
    update();
}


void ViewLabel::setIsoLineColor( const QColor& col ){

    if( col==m_isoLineColor) return;

    m_isoLineColor=col;

    update();

    emit isoLineColorChanged( col );
}


void ViewLabel::setIsoLineWidth( int w){

    if(w==m_isoLineWidth) return;

    m_isoLineWidth=w;

    update();

    emit isoLineWidthChanged(w);
}


void ViewLabel::setHighlightCDPColor( QColor col ){

    if( col==m_highlightCDPColor) return;

    m_highlightCDPColor=col;

    update();

    emit highlightCDPColorChanged(col );
}


void ViewLabel::setHighlightCDPSize( int w){

    if(w==m_highlightCDPSize) return;

    m_highlightCDPSize=w;

    update();

    emit highlightCDPSizeChanged(w);
}

void ViewLabel::paintEvent( QPaintEvent* ev){


    QLabel::paintEvent(ev);

    Q_ASSERT( m_view );
    Q_ASSERT( m_view->horizontalRuler());
    Q_ASSERT( m_view->verticalRuler());

    if( !m_view->grid())return;


    QPainter painter(this);



    QPen gridPen( Qt::black);
    gridPen.setWidth(0);
    painter.setPen(gridPen);
    drawVerticalGrid( painter );
    drawHorizontalGrid( painter );


    QPen isoPen( m_isoLineColor);
    isoPen.setWidth(m_isoLineWidth);
    painter.setPen(isoPen);
    painter.drawLines(paintSegs);

    drawHighlightedCDPs(painter);

    QPen linePen(Qt::red);
    linePen.setWidth(0);
    drawPolyline(painter, linePen);
}


void ViewLabel::resizeEvent(QResizeEvent*){

    m_view->setGridToImageTransform( m_view->computeGridToImageTransform() );

    updatePixmap();
    updateLineSegments();
}

void ViewLabel::mouseMoveEvent(QMouseEvent * ev){

    int x=ev->x();
    int y=ev->y();


    QPointF p=m_view->imageToGridTransform().map( QPointF(x,y));
    int i=std::round( p.y() );
    int j=std::round( p.x() );

    //std::cout<<"MO point="<<x<<"/"<<y<<" -> il="<<i<<" xl="<<j<<std::endl;

    emit mouseOver( i, j );
}


void ViewLabel::setImage( QImage img ){

    if( img==m_image ) return;

    m_image=img;

    emit imageChanged();


    updatePixmap();
}

void ViewLabel::setTransformationMode( Qt::TransformationMode m ){

    if( m== m_transformationMode ) return;

    m_transformationMode=m;

    emit transformationModeChanged(m);


    updatePixmap();
}

void ViewLabel::updatePixmap(){

    setPixmap( QPixmap::fromImage(m_image).scaled( size(),  Qt::IgnoreAspectRatio, m_transformationMode) );
}


void ViewLabel::updateLineSegments(){


    if( !m_view || !m_view->m_grid ) return;

    QTransform trans=m_view->gridToImageTransform();

/*
 * CANNOT use progress dialog because sometimes this is too slow and resizeevents are called when not finished
    QProgressDialog progress(QString("Updating isolines"), QString("Cancel"), 0, m_isoLineSegments.size() );
    progress.show();
    progress.setCancelButton(0);
    qApp->processEvents();
*/

    paintSegs.clear();
    paintSegs.reserve(m_isoLineSegments.size());
    for( int i=0; i<m_isoLineSegments.size(); i++ ){
        QLineF m=trans.map( m_isoLineSegments[i] );
        paintSegs.push_back( m);
       /* see above
        progress.setValue(i);
        qApp->processEvents();
        */

    }

}

void ViewLabel::drawHorizontalGrid( QPainter& painter){

    QScrollBar* sb=m_view->horizontalScrollBar();
    Ruler* ruler=m_view->horizontalRuler();

    Q_ASSERT( sb );
    Q_ASSERT( ruler);

    QVector< Ruler::Tick > ticks=ruler->computeTicks( sb->value(), sb->value() + sb->pageStep() );

    for( Ruler::Tick tick : ticks){
        int x=tick.coord();
        painter.drawLine( x, 0, x, height() );
    }

}

void ViewLabel::drawVerticalGrid( QPainter& painter){

    QScrollBar* sb=m_view->verticalScrollBar();
    Ruler* ruler=m_view->verticalRuler();

    Q_ASSERT( sb );
    Q_ASSERT( ruler);

    QVector< Ruler::Tick > ticks=ruler->computeTicks( sb->value(), sb->value() + sb->pageStep() );

    for( Ruler::Tick tick : ticks){
        int y=tick.coord();
        painter.drawLine( 0, y, width(), y );
    }
}


void ViewLabel::drawHighlightedCDPs( QPainter& painter){

    painter.save();

    //QBrush theBrush(m_highlightCDPColor);
    painter.setPen(m_highlightCDPColor);
    painter.setBrush(m_highlightCDPColor);
    QTransform trans=m_view->gridToImageTransform();

    QPainterPath path;
    for( SelectionPoint spoint : m_view->highlightedCDPs() ){

        QPointF point=trans.map( QPointF(spoint.xline, spoint.iline ) ); // IMPORTANT: internally inline on y-axis!!!!
        painter.drawEllipse(point, 0.5*m_highlightCDPSize, 0.5*m_highlightCDPSize);
    }

    painter.restore();
}

void ViewLabel::drawPolyline( QPainter& painter, const QPen& pen){

    painter.save();

    QTransform trans=m_view->gridToImageTransform();

    const QVector<QPoint>& polyline=m_view->polylineSelection();
    QPainterPath path;
    for( int i=0; i<polyline.size(); i++ ){

        const QPoint& inlineAndCrossline=polyline[i];
        QPointF point=trans.map( QPointF(inlineAndCrossline.y(), inlineAndCrossline.x() ) ); // IMPORTANT: internally inline on y-axis!!!!
        if( i>0 ){
            path.lineTo(point);
        }
        else{
            path.moveTo(point);
        }
    }

    painter.strokePath(path, pen);

    painter.restore();
}

Ruler::Ruler( GridView* parent, Orientation orient ):QWidget(parent), m_view(parent), m_orientation( orient){
}



void Ruler::paintEvent(QPaintEvent*){

    QPainter painter(this);

    painter.setPen(Qt::black);

    if( m_orientation==VERTICAL_RULER){
        drawVertical( painter );
    }
    else{
        drawHorizontal( painter );
    }

}



int Ruler::tickIncrement(int size_pix, int size_grid, int min_distance)const{

    double pix_per_unit=double(size_pix)/size_grid;
    int incr= std::pow(10,std::floor(std::log10(min_distance/pix_per_unit)));

    if( pix_per_unit>=min_distance){
        incr=1;
    }
    else if( 2*incr*pix_per_unit>=min_distance){
        incr*=2;
    }
    else  if( 5*incr*pix_per_unit>=min_distance){
        incr*=5;
    }
    else  if( 10*incr*pix_per_unit>=min_distance){
        incr*=10;
    }
    else{

        qWarning("No tick increment found!");
        incr=-1;
    }

    return incr;
}

int Ruler::tickIncrement()const{

    if( !m_view || !m_view->m_grid) return -1;

    Grid2D<float>::bounds_type bounds=m_view->grid()->bounds();
    int size_pix;
    int size_grid;
    int min_distance;

    if( m_orientation==VERTICAL_RULER){

        size_pix=m_view->m_label->height();
        size_grid=(m_view->m_inlineOrientation==AxxisOrientation::Horizontal) ? bounds.height() : bounds.width();
        min_distance=TICK_LABEL_DY;
    }
    else{

        size_pix=m_view->m_label->width();
        size_grid=( m_view->m_inlineOrientation==AxxisOrientation::Horizontal)? bounds.width() : bounds.height();
        min_distance=TICK_LABEL_DX;
    }

    return tickIncrement( size_pix, size_grid, min_distance);
}


QVector< Ruler::Tick > Ruler::computeTicks( int coord1, int coord2 )const{

    QVector< Tick > ticks;

    const QTransform& grid2img=m_view->gridToImageTransform();
    const QTransform& img2grid=m_view->imageToGridTransform();

    if( m_orientation==VERTICAL_RULER){

       QPointF startP( 0, coord1);
       QPointF endP( 0, coord2);
       int incr=tickIncrement();

       if( m_view->inlineOrientation() == AxxisOrientation::Horizontal ){   // inlines on vertical label

           int startVal=img2grid.map( startP ).y();
           int endVal=img2grid.map(endP).y();
           if( startVal > endVal ){
               std::swap( startVal, endVal );
           }

           for( int il=incr*(startVal/incr+1); il<endVal; il+=incr){

               int coord=std::round( grid2img.map( QPointF( 0, il )).y() );
               ticks.push_back( Tick( coord, il ));
           }

       }
       else{                                                                // crosslines on vertical label

           int startVal=img2grid.map( startP ).x();
           int endVal=img2grid.map(endP).x();
           if( startVal > endVal ){
               std::swap( startVal, endVal );
           }

           for( int xl=incr*(startVal/incr+1); xl<endVal; xl+=incr){

               int coord=std::round( grid2img.map( QPointF( xl, 0 )).y() );
               ticks.push_back( Tick( coord, xl ));
           }
       }
    }

    else{                                                                   // Horizontal ruler

        QPointF startP( coord1, 0);
        QPointF endP( coord2, 0);

        int incr=tickIncrement();

        if( m_view->inlineOrientation() == AxxisOrientation::Horizontal ){   // crosslines on horizontal label

            int startVal=img2grid.map( startP ).x();
            int endVal=img2grid.map(endP).x();
            if( startVal > endVal ){
                std::swap( startVal, endVal );
            }

            for( int xl=incr*(startVal/incr+1); xl<endVal; xl+=incr){

                int coord=std::round( grid2img.map( QPointF( xl, 0 )).x() );
                ticks.push_back( Tick( coord, xl ));
            }

        }
        else{                                                                // inlines on horizontal label

            int startVal=img2grid.map( startP ).y();
            int endVal=img2grid.map(endP).y();
            if( startVal > endVal ){
                std::swap( startVal, endVal );
            }

            for( int il=incr*(startVal/incr+1); il<endVal; il+=incr){

                int coord=std::round( grid2img.map( QPointF( 0, il )).x() );
                ticks.push_back( Tick( coord, il ));
            }
        }
     }

    return ticks;

}


void Ruler::drawVertical( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    QScrollBar* sb=m_view->verticalScrollBar();
    if( !sb ){
        qDebug("sb == 0");
        return;
    }

    if( !m_view->grid() ){
        qDebug("grid == 0");
        return;
    }


    const QString& text = ( m_view->inlineOrientation() == AxxisOrientation::Horizontal ) ?
                        m_view->inlineAxxisLabel() : m_view->crosslineAxxisLabel();

    painter.save();
    painter.rotate( -90);
   // painter.translate( 0, -width());
    QSize text_size=painter.fontMetrics().size( Qt::TextSingleLine, text);

    painter.drawText( -( height() + text_size.width() )/2, text_size.height(), text );         // center vertically
    painter.restore();


    QVector<Tick> ticks=computeTicks( sb->value(), sb->value() + sb->pageStep() );

    int x1=width();
    int x2=x1-1.5*TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int y=tick.coord() - sb->value();

        painter.drawLine( x1, y , x2, y );
        painter.drawText( 0, y - TICK_LABEL_DY/2, x2, TICK_LABEL_DY, Qt::AlignRight|Qt::AlignVCenter, QString::number( tick.value()) );

    }

}

void Ruler::drawHorizontal( QPainter& painter ){


    if( !m_view){
        qDebug("m_view == 0");
        return;
    }

    QScrollBar* sb=m_view->horizontalScrollBar();
    if( !sb ){
        qDebug("sb == 0");
        return;
    }

    if( !m_view->grid() ){
        qDebug("grid == 0");
        return;
    }

    const QString& text = ( m_view->inlineOrientation() == AxxisOrientation::Vertical ) ?
                        m_view->inlineAxxisLabel() : m_view->crosslineAxxisLabel();
    QSize text_size=painter.fontMetrics().size( Qt::TextSingleLine, text);
    painter.drawText( ( width() - text_size.width() ) / 2, text_size.height(), text );  // center horizontally


    QVector<Tick> ticks=computeTicks( sb->value(), sb->value() + sb->pageStep() );

    int y1=height();
    int y2=y1-1.5*TICK_MARK_SIZE;

    for( Tick tick: ticks ){

        int x=tick.coord() - sb->value();
        painter.drawLine( x, y1, x, y2 );
        painter.drawText( x - TICK_LABEL_DX/2, 0, TICK_LABEL_DX, y2, Qt::AlignBottom|Qt::AlignHCenter, QString::number(tick.value()) );
    }

}


