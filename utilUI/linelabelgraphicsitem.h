#ifndef LINELABELGRAPHICSITEM_H
#define LINELABELGRAPHICSITEM_H

#include<QGraphicsItem>
#include<QLineF>
#include<QFont>


// font sizes must be set in pixel size, if not point size is used as pixel size
class LineLabelGraphicsItem : public QGraphicsItem{

public:

    LineLabelGraphicsItem( QLineF line, QString text, QFont font=QFont("Helvetica [Cronyx]", 12, QFont::Bold) );

    QLineF line()const{
        return m_line;
    }

    QString text()const{
        return m_text;
    }

    QFont font()const{
        return m_font;
    }

    Qt::AlignmentFlag vAlign()const{
        return m_valign;
    }

    Qt::AlignmentFlag hAlign()const{
        return m_halign;
    }

    qreal padX()const{
        return m_padX;
    }

    qreal padY()const{
        return m_padY;
    }


    void setLine( QLineF);
    void setText( QString);
    void setFont( QFont );
    void setVAlign(Qt::AlignmentFlag);
    void setHAlign(Qt::AlignmentFlag);
    void setAlignments( Qt::AlignmentFlag horizontal, Qt::AlignmentFlag vertical );
    void setPadX(qreal);
    void setPadY(qreal);
    void setPadding(qreal px, qreal py);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:

    void updateLine();

    QLineF m_line;
    QString m_text;
    QFont m_font;
    Qt::AlignmentFlag m_valign=Qt::AlignBottom;
    Qt::AlignmentFlag m_halign=Qt::AlignHCenter;
    qreal m_padX=0;
    qreal m_padY=0;
};

#endif // LINELABELGRAPHICSITEM_H
