#include "gridview.h"

#include<QScrollBar>
#include<QPainter>

//#include<grid2d.h>
//using Grid=Grid2D<double>;

#include<iostream>

GridView::GridView(QWidget* parent):QScrollArea(parent), m_map(0,255)
{
    m_label = new QLabel( this );
    m_label->setBackgroundRole(QPalette::Base);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_label->setScaledContents(true);

    setWidget( m_label);

    //setWidgetResizable(true);

    //setUpdatesEnabled( false );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    setViewportMargins( RULER_WIDTH, RULER_HEIGHT, 0, 0);

    m_leftRuler=new Ruler( this, Ruler::VERTICAL_RULER);
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), m_leftRuler, SLOT( update()) );

    m_topRuler=new Ruler( this, Ruler::HORIZONTAL_RULER);
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), m_topRuler, SLOT( update()) );

    updateGeometry();
}

GridView::~GridView(){
}


void GridView::setGrid( std::shared_ptr<Grid2D> g){
    //if( g==m_grid) return;
    m_grid=g;
    emit(gridChanged(g));
    updatePixmap();
}

void GridView::setColorMapping( const std::pair<double,double>& m){
    if(m==m_map) return;
    m_map=m;
    emit(colorMappingChanged(m));
    updatePixmap( true );
}

void GridView::setColorTable(const ColorTable & ct){

    if( ct.size()>MAX_COLORS || ct.size()==0){
        qWarning("Trying to set colortable with wrong number of colors! Ignoring...");
        return;
    }

    if( ct==m_colorTable) return;
    m_colorTable=ct;
    emit( colorTableChanged(ct));
    updatePixmap( true );
}


void GridView::setScale( double s ){

    if( s==m_scale ) return;

    double factor=s/m_scale;

    Q_ASSERT( m_label->pixmap());
    Q_ASSERT( !m_label->pixmap()->isNull());
    //scaleFactor *= factor;
    m_label->resize( s * m_label->pixmap()->size());

    adjustScrollBar( horizontalScrollBar(), factor);
    adjustScrollBar( verticalScrollBar(), factor);



    m_scale=s;

    m_leftRuler->update();
    m_topRuler->update();

    emit scaleChanged(s);
}

void GridView::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void GridView::updatePixmap( bool keepSize){

    if( !m_label){
        qWarning("m_label==0");
        return;
    }

    if( !m_label->pixmap()){
        qWarning("pixmap==0");
        //return;
    }

    if( !m_grid ||  m_grid->bounds().width()<1 || m_grid->bounds().height()<1 || m_map.first==m_map.second || m_colorTable.empty()){
        qWarning("Illegal grid or mapping params in updatepixmap!");
        m_label->setPixmap(QPixmap());           //null pixmap
        return;
    }

    QImage img=grid2image();


    m_label->setPixmap(QPixmap::fromImage(img));

    if( !keepSize ){
        m_scale=1;
        m_label->resize(m_label->pixmap()->size());
    }
}


void GridView::updateLayout(){

    setUpdatesEnabled( false );

    m_leftRuler->setGeometry( 0, viewport()->y(), RULER_WIDTH, viewport()->height() );
    m_topRuler->setGeometry( viewport()->x(), 0, viewport()->width(), RULER_HEIGHT );

    setUpdatesEnabled( true );
}

void GridView::resizeEvent(QResizeEvent *ev){

  //m_label->setMinimumSize( viewport()->size());

    updateLayout();

}


QImage GridView::grid2image(){

    QImage img( m_grid->bounds().height(), m_grid->bounds().width(), QImage::Format_Indexed8);

    img.setColorCount( m_colorTable.size());
    img.setColorTable( m_colorTable);

    for( Grid2D::index_type i=m_grid->bounds().y1(); i<=m_grid->bounds().y2(); i++){

        for( Grid2D::index_type j=m_grid->bounds().x1(); j<=m_grid->bounds().x2(); j++){

            Grid2D::value_type v=(*m_grid)( i, j);
            v=(v-m_map.first)/(m_map.second-m_map.first);
            if(v<0) v=0;
            if(v>1) v=1;
            img.setPixel( i-m_grid->bounds().y1(), j-m_grid->bounds().x1(), static_cast<int>((m_colorTable.size()-1)*v) );
        }
    }

    return img;
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

void Ruler::computeCurrentTicks( int first_val, int end_val, int incr){

    m_currentTicks.clear();

    for( int i=first_val; i<end_val; i+=incr ){
        m_currentTicks.push_back(i);
    }

}

void Ruler::drawVertical( QPainter& painter ){

    int incr=25;

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

    Grid2D::bounds_type bounds=m_view->grid()->bounds();
    int size_pix=m_view->m_label->height();
    int size_grid=bounds.height();
    double pix_per_unit=double(size_pix)/size_grid;

    int start_val=sb->value()/pix_per_unit+bounds.y1();
    int end_val=start_val+sb->pageStep()/pix_per_unit;
    if( end_val>bounds.y2()) end_val=bounds.y2();

    std::cout<<"size_pix="<<size_pix<<std::endl;
    std::cout<<"size_grid="<<size_grid<<std::endl;
    std::cout<<"pix_per_unit="<<pix_per_unit<<std::endl;
    std::cout<<"start_val="<<start_val<<std::endl;
    std::cout<<"end_val="<<end_val<<std::endl;

    int x1=width();
    int x2=x1-1.5*TICK_MARK_SIZE;
    int first_val=incr*(1+start_val/incr);

    computeCurrentTicks( first_val, end_val, incr);

    for( int i : m_currentTicks ){

        double y=(i-start_val)*pix_per_unit;
        painter.drawLine( x1, y, x2, y );
        painter.drawText( 0, y - TICK_LABEL_DY/2, x2, TICK_LABEL_DY, Qt::AlignRight|Qt::AlignVCenter, QString::number(i) );
    }

}

void Ruler::drawHorizontal( QPainter& painter ){

    int incr=25;

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

    Grid2D::bounds_type bounds=m_view->grid()->bounds();
    int size_pix=m_view->m_label->width();
    int size_grid=bounds.width();
    double pix_per_unit=double(size_pix)/size_grid;

    int start_val=sb->value()/pix_per_unit+bounds.x1();
    int end_val=start_val+sb->pageStep()/pix_per_unit;
    if( end_val>bounds.x2()) end_val=bounds.x2();
/*
    std::cout<<"size_pix="<<size_pix<<std::endl;
    std::cout<<"size_grid="<<size_grid<<std::endl;
    std::cout<<"pix_per_unit="<<pix_per_unit<<std::endl;
    std::cout<<"start_val="<<start_val<<std::endl;
    std::cout<<"end_val="<<end_val<<std::endl;
*/
    int y1=height();
    int y2=y1-1.5*TICK_MARK_SIZE;
    int first_val=incr*(1+start_val/incr);
    computeCurrentTicks( first_val, end_val, incr);

    for( int i : m_currentTicks ){

        double x=(i-start_val)*pix_per_unit;
        painter.drawLine( x, y1, x, y2 );
        painter.drawText( x - TICK_LABEL_DX/2, 0, TICK_LABEL_DX, y2, Qt::AlignBottom|Qt::AlignHCenter, QString::number(i) );
    }

}



