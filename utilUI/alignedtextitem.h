#ifndef ALIGNEDTEXTITEM_H
#define ALIGNEDTEXTITEM_H

#include <QGraphicsSimpleTextItem>

class AlignedTextItem
    : public QGraphicsSimpleTextItem
{
public:
    AlignedTextItem( const QString & text, Qt::Alignment flags, QGraphicsItem * pParent = 0 );
    AlignedTextItem( const QString & text, QGraphicsItem * pParent = 0 );
    AlignedTextItem( QGraphicsItem * pParent = 0 );

    ~AlignedTextItem();

    QRectF boundingRect() const;
    void paint( QPainter * pPainter, const QStyleOptionGraphicsItem * pOption, QWidget * pWidget = 0 );

    Qt::Alignment alignment() const;
    void setAlignment( Qt::Alignment flags );

private:
    Qt::Alignment	mFlags;
    mutable QRectF	mBounds;
};


#endif // ALIGNEDTEXTITEM_H
