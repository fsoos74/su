#include "wellitem.h"

#include<QPen>
#include<QBrush>
#include<QFont>
#include<QVector2D>

WellItem::WellItem( QGraphicsItem* parent, QObject* parentObject) :
    QObject(parentObject),
    QGraphicsEllipseItem( -2, -2, 2, 2),
    m_textItem(new AlignedTextGraphicsItem("", Qt::AlignHCenter|Qt::AlignTop,0,this)),
    m_size(2)
{
    QPen pen(Qt::black, 1);
    pen.setCosmetic(true);
    setPen(pen);
    QBrush brush(Qt::darkGreen);
    setBrush(brush);

    QFont font("Times",8,QFont::Normal);
    m_textItem->setFont(font);
    //m_textItem->moveBy(m_size, m_size);
    m_textItem->setZValue(1000);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    //setHandlesChildEvents(true);
}


int WellItem::size()const{
    return m_size;
}

QFont WellItem::font()const{
    return m_textItem->font();
}

QString WellItem::text()const{
    return m_textItem->text();
}

/*
QPen WellItem::pen()const{
    return m_ellipseItem->pen();
}

QBrush WellItem::brush()const{
    return m_ellipseItem->brush();
}

*/

void WellItem::setInfo(const WellInfo & info){
    m_info=info;
    setPos(QPointF(info.x(), info.y()));
    setText(labelText());
}

void WellItem::setPath(std::shared_ptr<WellPath> path){
    if( path==m_path) return;
    m_path=path;
    update();
}

void WellItem::setRefDepth(qreal depth){

    if( depth==m_refDepth) return;

    m_refDepth=depth;

    if( m_path ){
        setPos( m_path->locationAtZ(-depth) );
    }
    else{
        setPos(QPointF(m_info.x(), m_info.y()));
    }
}

void WellItem::setLabelType(LabelType t){
    if( t==m_labelType) return;
    m_labelType=t;
    setText(labelText());
}

void WellItem::setSize(int s){

    m_size=s;

    prepareGeometryChange();

    QRectF rItem( -s/2, -s/2, 2*s, 2*s);
    //m_ellipseItem->setRect(rItem);
    setRect(rItem);
    m_textItem->setPos(m_size,m_size);
}

void WellItem::setText(QString text){
    m_textItem->setText(text);
}

void WellItem::setFont(QFont f){
    m_textItem->setFont(f);
}

/*
void WellItem::setPen(const QPen &pen){
    m_ellipseItem->setPen(pen);
}

void WellItem::setBrush(const QBrush & brush){
    m_ellipseItem->setBrush(brush);
}
*/

QString WellItem::labelText(){
    return (m_labelType==LabelType::UWI)?m_info.uwi() : m_info.name();
}
