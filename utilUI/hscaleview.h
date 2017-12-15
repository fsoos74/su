#ifndef HSCALEVIEW_H
#define HSCALEVIEW_H

#include <QObject>
#include <QFont>
#include "axis.h"
#include<axisview.h>

class HScaleView : public AxisView
{
Q_OBJECT
public:
    HScaleView(QWidget* parent, Qt::Alignment=Qt::AlignBottom);

    //Qt::Alignment alignment()const{
    //    return m_alignment;
    //}

protected:
    void mouseDoubleClickEvent(QMouseEvent*);
    void  drawBackground(QPainter *painter, const QRectF &rect)override;
    void refreshScene()override;

private:
    void drawTop(QPainter *painter, const QRectF &rect);
    void drawBottom(QPainter *painter, const QRectF &rect);

    //Qt::Alignment m_alignment;
    QFont m_labelFont = QFont("Helvetica [Cronyx]", 11, QFont::Bold);
    QFont m_mainTickFont = QFont("Helvetica [Cronyx]", 10, QFont::Bold);
    QFont m_subTickFont = QFont("Helvetica [Cronyx]", 10);
};

#endif // HSCALEVIEW_H
