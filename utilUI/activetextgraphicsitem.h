#ifndef ACTIVETEXTGRAPHICSITEM_H
#define ACTIVETEXTGRAPHICSITEM_H

#include "alignedtextgraphicsitem.h"

#include<functional>

class ActiveTextGraphicsItem : public AlignedTextGraphicsItem{

public:

    ActiveTextGraphicsItem( QString text, size_t id, QGraphicsItem* parent=0);

    int id()const{
        return m_id;
    }

    void setId(int);

    void setCallback( std::function<void(int)>);

protected:

    void hoverEnterEvent(QGraphicsSceneHoverEvent *);

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* );


    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w);

private:

    bool m_active=false;
    int m_id=-1;
    std::function<void(size_t)> m_callback;
};


#endif // ACTIVETEXTGRAPHICSITEM_H
