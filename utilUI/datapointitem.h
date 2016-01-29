#ifndef DATAPOINTITEM_H
#define DATAPOINTITEM_H

#include<QGraphicsItem>
#include<QColor>

class DatapointItem : public QGraphicsItem
{
public:

    const int PEN_WIDTH=1;

    DatapointItem(int regular_size=9, qreal activeSizeFactor=1.5,
                  const QColor& regularColor=Qt::blue, const QColor& selectedColor=Qt::red);

    int regularSize()const{
        return m_regularSize;
    }

    qreal activeSizeFactor()const{
        return m_activeSizeFactor;
    }

    const QColor& regularColor()const{
        return m_regularColor;
    }

    const QColor& selectedColor()const{
        return m_selectedColor;
    }

    void setRegularSize(int);
    void setActiveSizeFactor(qreal);
    void setRegularColor(const QColor&);
    void setSelectedColor(const QColor&);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void 	hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void 	hoverLeaveEvent(QGraphicsSceneHoverEvent * event);


private:
    int m_regularSize=10;
    qreal m_activeSizeFactor=1.5;
    QColor m_regularColor=Qt::blue;
    QColor m_selectedColor=Qt::red;
    int m_size;
};


#endif // DATAPOINTITEM_H
