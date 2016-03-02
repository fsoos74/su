#ifndef ALIGNEDTEXTGRAPHICSITEM_H
#define ALIGNEDTEXTGRAPHICSITEM_H

#include<QGraphicsItem>
#include<QFont>

class AlignedTextGraphicsItem : public QGraphicsItem{

public:

    AlignedTextGraphicsItem( QString text, QGraphicsItem* parent=0);

    const QString& text()const{
        return m_text;
    }

    const QFont& font()const{
        return m_font;
    }

    const Qt::Alignment hAlign()const{
        return m_halign;
    }

    const Qt::Alignment vAlign()const{
        return m_valign;
    }

    void setText( QString& t);

    void setFont(QFont font);

    void setHAlign( Qt::Alignment a);

    void setVAlign( Qt::Alignment a);

    QRectF boundingRect()const;

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

private:

    QString m_text;
    QFont m_font;
    Qt::Alignment m_halign=Qt::AlignHCenter;
    Qt::Alignment m_valign=Qt::AlignVCenter;
};

#endif // ALIGNEDTEXTGRAPHICSITEM_H
