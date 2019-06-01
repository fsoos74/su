#ifndef VSCALEVIEW_H
#define VSCALEVIEW_H

#include <QObject>
#include <QFont>
#include "axis.h"
#include<axisview.h>
class VScaleView : public AxisView
{
    Q_OBJECT
public:
    VScaleView(QWidget* parent=nullptr, Qt::Alignment align=Qt::AlignLeft);

    QColor color()const{
        return m_color;
    }

    QFont labelFont()const{
        return m_labelFont;
    }

    QFont mainTickFont()const{
        return m_mainTickFont;
    }

    QFont subTickFont()const{
        return m_subTickFont;
    }

public slots:
    void setColor(QColor);
    void setLabelFont(QFont);
    void setMainTickFont(QFont);
    void setSubTickFont(QFont);

protected:
    void mouseDoubleClickEvent(QMouseEvent*);
    void  drawBackground(QPainter *painter, const QRectF &rect)override;
    void refreshScene()override;

private:

    void  drawLeft(QPainter *painter, const QRectF &rect);
    void  drawRight(QPainter *painter, const QRectF &rect);

    //Qt::Alignment m_alignment;
    QColor m_color=Qt::darkGray;
    QFont m_labelFont = QFont("Helvetica [Cronyx]", 11, QFont::Bold);
    QFont m_mainTickFont = QFont("Helvetica [Cronyx]", 10, QFont::Bold);
    QFont m_subTickFont = QFont("Helvetica [Cronyx]", 10);
};

#endif // VSCALEVIEW_H
