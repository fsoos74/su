#ifndef POINTITEM_H
#define POINTITEM_H

#include<QGraphicsItemGroup>
#include<QGraphicsEllipseItem>
#include<QFont>
#include"alignedtextgraphicsitem.h"
#include<memory>
#include<QObject>

class PointItem : public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:

    PointItem( QGraphicsItem* parentGraphicsItem=nullptr, QObject* parentQObject=nullptr);


    QColor symbolColor()const{
        return m_symbolColor;
    }

    QFont font()const;
    int size() const;
    QString text()const;

public slots:

    void setSymbolColor(QColor);
    void setFont(QFont);
    void setSize(int);
    void setText(QString);

private:

    int m_size=3;
    QColor m_symbolColor=Qt::darkRed;

    QGraphicsEllipseItem* m_symbol=nullptr;
    AlignedTextGraphicsItem* m_label=nullptr;
};

#endif // POINTITEM_H
