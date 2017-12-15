#include "alignedtextitem.h"
#include <QPainter>

AlignedTextItem::AlignedTextItem( const QString & text /*= QString()*/, Qt::Alignment flags /*= Qt::AlignTop | Qt::AlignLeft*/, QGraphicsItem * pParent /*= 0 */ )
    : QGraphicsSimpleTextItem( text, pParent )
    , mFlags( flags )
{
    setFlag( ItemSendsGeometryChanges );
    mBounds = QGraphicsSimpleTextItem::boundingRect();
}

AlignedTextItem::AlignedTextItem( const QString & text /*= QString()*/, QGraphicsItem * pParent /*= 0 */ )
    : QGraphicsSimpleTextItem( text, pParent )
    , mFlags( Qt::AlignTop | Qt::AlignLeft )
{
    setFlag( ItemSendsGeometryChanges );
    mBounds = QGraphicsSimpleTextItem::boundingRect();
}

AlignedTextItem::AlignedTextItem( QGraphicsItem * pParent /*= 0 */ )
    : QGraphicsSimpleTextItem( pParent )
    , mFlags( Qt::AlignTop | Qt::AlignLeft )
{
    setFlag( ItemSendsGeometryChanges );
    mBounds = QGraphicsSimpleTextItem::boundingRect();
}

AlignedTextItem::~AlignedTextItem()
{
}

Qt::Alignment AlignedTextItem::alignment() const
{
    return mFlags;
}

void AlignedTextItem::setAlignment( Qt::Alignment flags )
{
    mFlags = flags;
}

QRectF AlignedTextItem::boundingRect() const
{
    mBounds = QGraphicsSimpleTextItem::boundingRect();
    QPointF	offset( 0.0, 0.0 );
    if ( mFlags.testFlag( Qt::AlignRight ) )
        offset.setX( -mBounds.width() );
    else if ( mFlags.testFlag( Qt::AlignHCenter ) )
        offset.setX( -mBounds.width() / 2.0 );

    if ( mFlags.testFlag( Qt::AlignBottom ) )
        offset.setY( -mBounds.height() );
    else if ( mFlags.testFlag( Qt::AlignVCenter ) )
        offset.setY( -mBounds.height() / 2.0 );

    mBounds.translate( offset );

    return mBounds;
}

void AlignedTextItem::paint( QPainter * pPainter, const QStyleOptionGraphicsItem * pOption, QWidget * pWidget /*= 0 */ )
{
    pPainter->translate( mBounds.left(), mBounds.top() );
    QGraphicsSimpleTextItem::paint( pPainter, pOption, pWidget );
}
