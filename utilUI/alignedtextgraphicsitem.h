#ifndef ALIGNEDTEXTGRAPHICSITEM_H
#define ALIGNEDTEXTGRAPHICSITEM_H

#include<QGraphicsSimpleTextItem>


class AlignedTextGraphicsItem : public QGraphicsSimpleTextItem
{
public:
    AlignedTextGraphicsItem(QString text="", Qt::Alignment align=Qt::AlignCenter, double rot=0,
                            QGraphicsItem* parent=nullptr);
    QRectF boundingRect()const;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    Qt::Alignment m_align;
    double m_rot;
};

#endif // ALIGNEDTEXTGRAPHICSITEM_H
