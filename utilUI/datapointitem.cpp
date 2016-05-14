#include "datapointitem.h"

#include<QPainter>

DatapointItem::DatapointItem(int regularSize, qreal activeSizeFactor, const QColor& regularColor, const QColor& selectedColor):
    m_regularSize(regularSize),
    m_activeSizeFactor(activeSizeFactor),
    m_regularColor(regularColor),
    m_selectedColor(selectedColor),
    m_size(regularSize)
{
    // removed this to enhance speed when using alot of datapoints
   // XXX setAcceptHoverEvents(true);
}

void DatapointItem::setRegularSize(int size){
    prepareGeometryChange();
    m_regularSize=size;
    update();
}

void DatapointItem::setActiveSizeFactor(qreal factor){
    prepareGeometryChange();
    m_activeSizeFactor=factor;
    update();
}

void DatapointItem::setRegularColor(const QColor& color){
    m_regularColor=color;
    update();
}

void DatapointItem::setSelectedColor(const QColor& color){
    m_selectedColor=color;
    update();
}

QRectF DatapointItem::boundingRect() const
{
    return QRectF( -PEN_WIDTH-m_size/2, -PEN_WIDTH-m_size/2, 2*PEN_WIDTH+m_size, 2*PEN_WIDTH+m_size );
}

void DatapointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    painter->setPen(QPen(Qt::black, PEN_WIDTH));

    if( isSelected()){
        painter->setBrush(m_selectedColor);
    }
    else{
        painter->setBrush( m_regularColor);
    }

    painter->drawEllipse(QRect(-m_size/2, -m_size/2, m_size, m_size));
}

void 	DatapointItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
    prepareGeometryChange();
    m_size=m_activeSizeFactor * m_regularSize;
    update();
}

void 	DatapointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
    prepareGeometryChange();
    m_size=m_regularSize;
    update();
}
