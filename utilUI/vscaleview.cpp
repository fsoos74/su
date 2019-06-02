#include "vscaleview.h"
#include<alignedtextitem.h>
#include<axisconfigdialog.h>
#include<cmath>



VScaleView::VScaleView(QWidget* parent, Qt::Alignment alignment):AxisView(parent)//, m_alignment(alignment)
{
    setAlignment(alignment);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void VScaleView::setColor(QColor color){
    if(color==m_color) return;
    m_color=color;
    update();
}

void VScaleView::setLabelFont(QFont font){
    if(font==m_labelFont) return;
    m_labelFont=font;
    update();
}

void VScaleView::setMainTickFont(QFont font){
    if(font==m_mainTickFont) return;
    m_mainTickFont=font;
    update();
}

void VScaleView::setSubTickFont(QFont font){
    if(font==m_subTickFont) return;
    m_subTickFont=font;
    update();
}

void VScaleView::mouseDoubleClickEvent(QMouseEvent *){
    AxisConfigDialog::configAxis( zAxis(), tr("Configure Z-Axis") );
}

void VScaleView::drawBackground(QPainter *painter, const QRectF &rect){

    //painter->eraseRect(rect);
    painter->fillRect(rect,this->palette().background());

    painter->save();

    painter->resetTransform();      // now we work in pixel!!!

    if( alignment() & Qt::AlignLeft ){
        drawLeft(painter, rect);
    }
    else{
        drawRight(painter, rect);
    }

    painter->restore();
}

void VScaleView::drawLeft(QPainter *painter, const QRectF &rect){

    // always draw label
    QString ltext=zAxis()->name();
    if(!zAxis()->unit().isEmpty()){
        ltext+=QString(" [%1]").arg(zAxis()->unit());
    }
    if( !ltext.isEmpty()){
        painter->setFont(m_labelFont);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(ltext);
        painter->rotate(90);
        painter->drawText( height()/2 - br.width()/2, -br.height(), ltext);
        painter->rotate(-90);
    }

    QPen mainTickPen(m_color, 2 );
    QPen subTickPen(m_color, 1);
    mainTickPen.setCosmetic(true);
    subTickPen.setCosmetic(true);
    int mainTickLen=5;
    int subTickLen=3;

    // draw vertical line on left side
    painter->setPen(subTickPen);
    painter->drawLine( width()-1, 0, width()-1, height());
    auto zticks=zAxis()->computeTicks( zAxis()->toAxis(rect.top()), zAxis()->toAxis(rect.bottom() ) );
    for( auto tick : zticks){
        painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
        int len = (tick.type == Axis::Tick::Type::Main) ? mainTickLen : subTickLen;
        qreal z=zAxis()->toScene(tick.value);
        auto prs = QPointF(rect.right(), z);
        auto prv = mapFromScene( prs );
        auto plv = QPoint( prv.x() - len, prv.y() );
        auto pls = mapToScene( plv );
        painter->drawLine(plv, prv);

        if( tick.label.isEmpty()) continue;

        QFont font = (tick.type == Axis::Tick::Type::Main ) ? m_mainTickFont : m_subTickFont;
        painter->setFont(font);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(tick.label);
        int y=std::min(prv.y() + br.height()/2, height());
        painter->drawText( prv.x() - 2*mainTickLen - br.width(), y , tick.label );
    }

}


void VScaleView::drawRight(QPainter *painter, const QRectF &rect){

    // always draw label
    QString ltext=zAxis()->name();
    if(!zAxis()->unit().isEmpty()){
        ltext+=QString(" [%1]").arg(zAxis()->unit());
    }
    if( !ltext.isEmpty()){
        painter->setFont(m_labelFont);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(ltext);
        painter->rotate(90);
        painter->drawText( height()/2 - br.width()/2, -width()+br.height(), ltext);
        painter->rotate(-90);
    }

    QPen mainTickPen(m_color, 2 );
    QPen subTickPen(m_color, 1);
    mainTickPen.setCosmetic(true);
    subTickPen.setCosmetic(true);
    int mainTickLen=5;
    int subTickLen=3;

    // draw vertical line on right side
    painter->setPen(subTickPen);
    painter->drawLine( 0, 0, 0, height());
    auto zticks=zAxis()->computeTicks( zAxis()->toAxis(rect.top()), zAxis()->toAxis(rect.bottom() ) );
    for( auto tick : zticks){
        painter->setPen( (tick.type == Axis::Tick::Type::Main ) ? mainTickPen : subTickPen);
        int len = (tick.type == Axis::Tick::Type::Main) ? mainTickLen : subTickLen;
        qreal z=zAxis()->toScene(tick.value);
        auto prs = QPointF(rect.left(), z);
        auto prv = mapFromScene( prs );
        auto plv = QPoint( prv.x() + len, prv.y() );
        auto pls = mapToScene( plv );
        painter->drawLine(plv, prv);

        if( tick.label.isEmpty()) continue;

        QFont font = (tick.type == Axis::Tick::Type::Main ) ? m_mainTickFont : m_subTickFont;
        painter->setFont(font);
        painter->setPen(QPen(m_color));
        auto br = painter->fontMetrics().boundingRect(tick.label);
        painter->drawText( prv.x() + 2*mainTickLen, prv.y() + br.height()/2 , tick.label );
    }

}



void VScaleView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    scene->setSceneRect(sceneRect());
    setScene(scene);
}
