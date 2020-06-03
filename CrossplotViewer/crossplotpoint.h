#ifndef CROSSPLOTPOINT_H
#define CROSSPLOTPOINT_H

#include<QGraphicsItem>

class CrossplotPoint : public QGraphicsItem
{
public:
    enum class Style{Circle, Square, Cross};
    CrossplotPoint(QGraphicsItem* parent=nullptr);
    Style style()const;
    void setStyle(Style);
    int size()const;
    void setSize(int);
    QColor color()const;
    void setColor(QColor);
    QColor selectedColor()const;
    void setSelectedColor(QColor);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)override;
private:
    Style mStyle=Style::Circle;
    int mSize=3;
    QColor mColor=Qt::blue;
    QColor mSelectedColor=Qt::red;
};

#endif // CROSSPLOTPOINT_H
