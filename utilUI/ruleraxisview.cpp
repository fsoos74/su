#include "ruleraxisview.h"

#include<QScrollBar>
#include<QResizeEvent>

RulerAxisView::RulerAxisView(QWidget* parent):AxisView(parent)
{
    m_vscaleView = new VScaleView(this, Qt::AlignLeft);
    m_vscaleView->setZAxis( zAxis());
    m_vscaleView->setZoomMode(AxisView::ZOOMMODE::Z);
    m_vscaleView->setCursorMode(AxisView::CURSORMODE::Z);
    m_vscaleView->setFrameShape(QGraphicsView::NoFrame );   // very important, otherwise views will be off a little

    m_hscaleView = new HScaleView(this, Qt::AlignBottom);
    m_hscaleView->setXAxis( xAxis());
    m_hscaleView->setCursorMode(AxisView::CURSORMODE::X);
    m_hscaleView->setZoomMode(AxisView::ZOOMMODE::X);
    m_hscaleView->setFrameShape(QGraphicsView::NoFrame );   // very important, otherwise views will be off a little

    // connect vertical scrollbars
    connect( m_vscaleView->verticalScrollBar(), SIGNAL(valueChanged(int)), verticalScrollBar(), SLOT(setValue(int)));
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), m_vscaleView->verticalScrollBar(), SLOT(setValue(int)));

    // connect horizontal scrollbars
    connect( m_hscaleView->horizontalScrollBar(), SIGNAL(valueChanged(int)), horizontalScrollBar(), SLOT(setValue(int)));
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), m_hscaleView->horizontalScrollBar(), SLOT(setValue(int)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    m_cornerWidget=new QWidget(this);

    updateLayout();
}

void RulerAxisView::setXAxis(Axis *axis){
    m_hscaleView->setXAxisNoDelete(axis);
    AxisView::setXAxis(axis);   // this deletesold axis
}

void RulerAxisView::setZAxis(Axis *axis){
    m_vscaleView->setZAxisNoDelete(axis);
    AxisView::setZAxis(axis);   // this deletesold axis
}

void RulerAxisView::setHScaleAlignment(Qt::Alignment a){
    if( a==m_hscaleView->alignment()) return;
    m_hscaleView->setAlignment(a);
    updateLayout();
}

void RulerAxisView::setVScaleAlignment(Qt::Alignment a){
    if( a==m_vscaleView->alignment()) return;
    m_vscaleView->setAlignment(a);
    updateLayout();
}

// this takes ownership of widget and deletes old corner widget!
void RulerAxisView::setCornerWidget(QWidget* w){
    if(w==m_cornerWidget) return;
    auto old=m_cornerWidget;
    m_cornerWidget=w;
    if(old) delete old;
    w->setParent(this);
    updateLayout();
}

void RulerAxisView::resizeEvent(QResizeEvent * event){
    AxisView::resizeEvent(event);
    updateLayout();
}


//#include<iostream>
void RulerAxisView::updateLayout(){

    const int RULER_WIDTH=120;
    const int RULER_HEIGHT=120;

    setUpdatesEnabled( false );

    QMargins margins;
    int vx,hy;

    if( m_vscaleView->alignment() & Qt::AlignLeft){
        margins.setLeft(RULER_WIDTH);
        margins.setRight(0);
        vx=0;
    }
    else{
        margins.setLeft(0);
        margins.setRight(RULER_WIDTH);
        vx=viewport()->width();
    }


    if( m_hscaleView->alignment() & Qt::AlignTop){
        margins.setTop(RULER_HEIGHT);
        margins.setBottom(0);
        hy=0;
    }
    else{
        margins.setTop(0);
        margins.setBottom(RULER_HEIGHT);
        hy=viewport()->height();
    }

    setViewportMargins( margins ); // RULER_WIDTH, 0, 0, RULER_HEIGHT);
    m_vscaleView->setGeometry( vx, viewport()->y(), RULER_WIDTH, viewport()->height() );
    m_hscaleView->setGeometry( viewport()->x(), hy, viewport()->width(), RULER_HEIGHT );
    m_cornerWidget->setGeometry( 0, 0, RULER_WIDTH, RULER_HEIGHT);
    setUpdatesEnabled( true );

    //std::cout<<"vp w="<<viewport()->width()<<" h="<<viewport()->height()<<std::endl;
    //std::cout<<"hs w="<<m_hscaleView->width()<<std::endl;
    //std::cout<<"vs h="<<m_vscaleView->height()<<std::endl<<std::flush;
}
