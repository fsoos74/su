#include "wellitem.h"

#include<QPen>
#include<QBrush>
#include<QFont>
#include<QVector2D>
#include<cmath>
#include<QMap>

namespace{
QMap<WellItem::LabelType,QString> lookup{
    {WellItem::LabelType::UWI, "UWI"},
    {WellItem::LabelType::WELL_NAME, "Name"}
};

}

QString toQString(WellItem::LabelType t){
    return lookup.value(t);
}

WellItem::LabelType toLabelType(QString str){
    return lookup.key(str);
}

WellItem::WellItem( QGraphicsItem* parent, QObject* parentObject) :
    QObject(parentObject),
    QGraphicsItemGroup(parent)
{
    m_symbol=new QGraphicsEllipseItem(-m_size,-m_size,2*m_size,2*m_size,this);
    m_symbol->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_symbol->setPen(Qt::NoPen);
    QBrush brush(Qt::darkGreen);
    m_symbol->setBrush(brush);

    m_label=new AlignedTextGraphicsItem("", Qt::AlignHCenter|Qt::AlignTop,0,this);
    QFont font("Times",8,QFont::Normal);
    m_label->setFont(font);
    m_label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_label->moveBy(0,m_size);
    //m_textItem->setZValue(1000);

    m_pathItem=new QGraphicsPathItem(this);
    QPen ppen(m_pathColor,1);
    ppen.setCosmetic(true);
    m_pathItem->setPen(ppen);
    updateLabel();
    updatePathItem();
}


int WellItem::size()const{
    return m_size;
}

QFont WellItem::font()const{
    return m_label->font();
}

QString WellItem::label()const{
    return m_label->text();
}

void WellItem::setInfo(const WellInfo & info){
    m_info=info;
    setPos(QPointF(info.x(), info.y()));
    updateLabel();
}

void WellItem::setPath(std::shared_ptr<WellPath> path){
    if( path==m_path) return;
    m_path=path;
    updatePathItem();
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
    updatePathItem();
}

void WellItem::setLabelType(LabelType t){
    if( t==m_labelType) return;
    m_labelType=t;
    updateLabel();
}

void WellItem::setSize(int s){

    m_size=s;

    prepareGeometryChange();

    QRectF rItem( -s/2, -s/2, 2*s, 2*s);
    m_symbol->setRect(rItem);
    //setRect(rItem);
    m_label->setPos(m_size,m_size);
}

void WellItem::setSymbolColor(QColor c){
    m_symbolColor=c;
    QBrush brush(m_symbolColor);
    m_symbol->setBrush(brush);
    update();
}

void WellItem::setPathColor(QColor c){
    m_pathColor=c;
    QPen ppen(m_pathColor,1);
    ppen.setCosmetic(true);
    m_pathItem->setPen(ppen);
    update();
}

void WellItem::setText(QString text){
    m_label->setText(text);
}

void WellItem::setFont(QFont f){
    m_label->setFont(f);
}

void WellItem::updateLabel(){
    QString text = (m_labelType==LabelType::UWI)?m_info.uwi() : m_info.name();
    m_label->setText(text);
}

void WellItem::updatePathItem(){

    QPainterPath path;
    if(!m_path){
        m_pathItem->setPath(path);
        return;
    }
    QPolygonF poly;
    auto x0=x();//m_path->at(0).x();
    auto y0=y();//m_path->at(0).y();
    for( int i=0; i<m_path->size(); i++ ){
        auto p3d=m_path->at(i);
        if( -p3d.z()<=m_refDepth ){		// z axis upwards, depth increases downwards
            poly.append(QPointF(p3d.x()-x0, p3d.y()-y0 ) );
        }
    }
    path.addPolygon(poly);
    m_pathItem->setPath(path);
    m_pathItem->update();

}
