#include "hscaleview.h"
#include<axisconfigdialog.h>
#include<cmath>
#include<QGraphicsScene>


HScaleView::HScaleView(QWidget* parent, Qt::Alignment alignment):AxisView(parent)//, m_alignment(alignment)
{
    setAlignment(alignment);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void HScaleView::setColor(QColor color){
    if(color==m_color) return;
    m_color=color;
    update();
}

void HScaleView::setLabelFont(QFont font){
    if(font==m_labelFont) return;
    m_labelFont=font;
    update();
}

void HScaleView::setMainTickFont(QFont font){
    if(font==m_mainTickFont) return;
    m_mainTickFont=font;
    update();
}

void HScaleView::setSubTickFont(QFont font){
    if(font==m_subTickFont) return;
    m_subTickFont=font;
    update();
}

void HScaleView::mouseDoubleClickEvent(QMouseEvent *){
    AxisConfigDialog::configAxis( xAxis(), tr("Configure X-Axis") );
}

void HScaleView::drawBackground(QPainter *painter, const QRectF &rect){

    //painter->eraseRect(rect);
    painter->fillRect(rect,this->palette().background());
    painter->save();

    painter->resetTransform();      // now we work in pixel!!!

    if( alignment() & Qt::AlignTop ){
        drawTop(painter, rect);
    }
    else{
        drawBottom(painter, rect);
    }

    painter->restore();
}


void HScaleView::drawBottom(QPainter *painter, const QRectF &rect){

    // always draw label
    QString ltext=xAxis()->name();
    if(!xAxis()->unit().isEmpty()){
        ltext+=QString(" [%1]").arg(xAxis()->unit());
    }
    if( !ltext.isEmpty()){
        painter->setFont(m_labelFont);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(ltext);
        painter->drawText( width()/2 - br.width()/2, height()-br.height(), ltext);
    }

    QPen mainTickPen(m_color, 2 );
    QPen subTickPen(m_color, 1);
    mainTickPen.setCosmetic(true);
    subTickPen.setCosmetic(true);
    int mainTickLen=5;
    int subTickLen=3;

    //draw horizontal line at top
    painter->setPen(subTickPen);
    painter->drawLine(0,0,width(),0);

    auto xticks=xAxis()->computeTicks( xAxis()->toAxis(rect.left()), xAxis()->toAxis(rect.right() ) );
    for( auto tick : xticks){

        painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
        int len = (tick.type == Axis::Tick::Type::Main) ? mainTickLen : subTickLen;
        qreal x=xAxis()->toScene(tick.value);
        auto pts = QPointF( x, rect.top() );
        auto ptv = mapFromScene( pts );
        auto pbv = QPoint( ptv.x(), ptv.y() + len );
        auto pbs = mapToScene( pbv );
        painter->drawLine(ptv, pbv);

        if( tick.label.isEmpty()) continue;

        QFont font = (tick.type == Axis::Tick::Type::Main ) ? m_mainTickFont : m_subTickFont;
        painter->setFont(font);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(tick.label);
        int xv = std::max(0,pbv.x() - br.height()/2);
        int yv = pbv.y()+mainTickLen;
        painter->translate(xv,yv);
        painter->rotate(90);
        painter->drawText(0,0,tick.label);
        painter->rotate(-90);
        painter->translate(-xv,-yv);

    }
}


void HScaleView::drawTop(QPainter *painter, const QRectF &rect){

    // always draw label
    QString ltext=xAxis()->name();
    if(!xAxis()->unit().isEmpty()){
        ltext+=QString(" [%1]").arg(xAxis()->unit());
    }
    if( !ltext.isEmpty()){
        painter->setFont(m_labelFont);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(ltext);
        painter->drawText( width()/2 - br.width()/2, br.height(), ltext);
    }

    QPen mainTickPen(m_color, 2 );
    QPen subTickPen(m_color, 1);
    mainTickPen.setCosmetic(true);
    subTickPen.setCosmetic(true);
    int mainTickLen=5;
    int subTickLen=3;

    //draw horizontal line at bottom
    painter->setPen(subTickPen);
    painter->drawLine(0,height()-1, width(), height()-1);

    auto xticks=xAxis()->computeTicks( xAxis()->toAxis(rect.left()), xAxis()->toAxis(rect.right() ) );
    for( auto tick : xticks){

        painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
        int len = (tick.type == Axis::Tick::Type::Main) ? mainTickLen : subTickLen;
        qreal x=xAxis()->toScene(tick.value);
        auto pts = QPointF( x, rect.bottom() );
        auto ptv = mapFromScene( pts );
        auto pbv = QPoint( ptv.x(), ptv.y() - len );
        auto pbs = mapToScene( pbv );
        painter->drawLine(ptv, pbv);

        if( tick.label.isEmpty()) continue;

        QFont font = (tick.type == Axis::Tick::Type::Main ) ? m_mainTickFont : m_subTickFont;
        painter->setFont(font);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(tick.label);
        int xv = std::max(0,pbv.x() - br.height()/2);
        int yv = pbv.y()-mainTickLen-br.width();
        painter->translate(xv,yv);
        painter->rotate(90);
        painter->drawText(0,0,tick.label);
        painter->rotate(-90);
        painter->translate(-xv,-yv);
    }
}



void HScaleView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);
    scene->setSceneRect(sceneRect());
    setScene(scene);
}
