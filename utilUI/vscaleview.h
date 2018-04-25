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

    //QSize sizeHint(){return QSize(50, 500);}
    //Qt::Alignment alignment()const{
    //    return m_alignment;
    //}

protected:
    void mouseDoubleClickEvent(QMouseEvent*);
    void  drawBackground(QPainter *painter, const QRectF &rect)override;
    void refreshScene()override;

private:

    void  drawLeft(QPainter *painter, const QRectF &rect);
    void  drawRight(QPainter *painter, const QRectF &rect);

    //Qt::Alignment m_alignment;
    QFont m_labelFont = QFont("Helvetica [Cronyx]", 11, QFont::Bold);
    QFont m_mainTickFont = QFont("Helvetica [Cronyx]", 10, QFont::Bold);
    QFont m_subTickFont = QFont("Helvetica [Cronyx]", 10);
};

#endif // VSCALEVIEW_H
