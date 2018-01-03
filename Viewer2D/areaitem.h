#ifndef AREAITEM_H
#define AREAITEM_H

#include <QObject>
#include <QGraphicsRectItem>
#include <avoproject.h>


class AreaItem : public QObject, public QGraphicsRectItem
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

    void setProject( AVOProject*);
    void setArea( QRect );  // inlines==x

public slots:

protected slots:

    void updateLabels();
    void updateFrame();
    void updateGeometry();

private:

    AVOProject* m_project=nullptr;
    QRect m_area;
    QVector<QGraphicsItem*> m_labelItems;
};

#endif // AREAITEM_H
