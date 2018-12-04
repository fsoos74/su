#ifndef AREAITEM_H
#define AREAITEM_H

#include <QObject>
#include <QGraphicsRectItem>
#include <avoproject.h>
#include <QGraphicsItemGroup>

class AreaItem : public QObject, public QGraphicsItemGroup
{
Q_OBJECT

public:

    AreaItem(AVOProject* project=nullptr, QGraphicsItem* parentGraphicsItemItem=nullptr, QObject* parentQObject=nullptr );

    AVOProject* project()const{
        return m_project;
    }

    QRect area()const{
        return m_area;
    }

    bool isShowLines()const{
        return m_showLines;
    }

    QColor outlineColor();
    QColor fillColor();
    qreal fillOpacity();

    void setProject( AVOProject*);

public slots:
    void setArea( QRect );  // inlines==x
    void setShowLines(bool);
    void setOutlineColor(QColor);
    void setFillColor(QColor);
    void setFillOpacity(qreal);

protected slots:
    void updateLabels();
    void updateFrame();
    void updateGeometry();

private:

    AVOProject* m_project=nullptr;
    QRect m_area;
    QGraphicsRectItem* m_outline;
    QGraphicsRectItem* m_fill;
    QVector<QGraphicsItem*> m_labelItems;

    bool m_showLines=true;
};

#endif // AREAITEM_H
