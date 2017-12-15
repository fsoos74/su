#ifndef AXISVIEW_H
#define AXISVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QPainterPath>
#include <QResizeEvent>
#include "axis.h"
#include <mousemode.h>
#include <memory>
#include <QRubberBand>

class AxisView : public QGraphicsView
{
Q_OBJECT

public:

    enum class ZOOMMODE{ NONE, X, Z, BOTH };
    enum class CURSORMODE{ NONE, X, Z, BOTH };
    enum class SELECTIONMODE{ None, SinglePoint, Line, Polygon };

    AxisView(QWidget* parent);

    ZOOMMODE zoomMode()const{
        return m_zoomMode;
    }

    CURSORMODE cursorMode()const{
        return m_cursorMode;
    }

    SELECTIONMODE selectionMode()const{
        return m_selectionMode;
    }

    Axis* xAxis()const{
        return m_xAxis;
    }

    Axis* zAxis()const{
        return m_zAxis;
    }

    bool xMesh()const{
        return m_xMesh;
    }

    bool zMesh()const{
        return m_zMesh;
    }

    MouseMode mouseMode()const{
        return m_mouseMode;
    }

    QPointF selectedPoint()const{
        return m_selectedPoint;
    }

    QLineF selectedLine()const{
        return m_selectedLine;
    }

    QPolygonF selectionPolygon()const{
        return m_selectionPolygon;
    }

    virtual void setXAxis(Axis*);
    virtual void setZAxis(Axis*);
    void setXAxisNoDelete(Axis*);
    void setZAxisNoDelete(Axis*);
    void setXMesh(bool);
    void setZMesh(bool);

    QRectF sceneRectFromAxes();
    QRectF viewRectFromAxes();
    QRectF sceneRectFromView();
    QRectF viewRectFromView();

public slots:
    void setZoomMode(ZOOMMODE);
    void setCursorMode(CURSORMODE);
    void setSelectionMode( SELECTIONMODE );
    void setMouseMode(MouseMode);
    void setSelectedLine(QLineF);
    void setSelectionPolygon( QPolygonF );
    void showXSelection(qreal, qreal);
    void showZSelection(qreal, qreal);
    void showXCursor(qreal);
    void showZCursor(qreal);
    void zoomFitWindow();
    void zoomBy(qreal factor);
    void zoomIn();
    void zoomOut();

signals:
    void mouseModeChanged(MouseMode);
    void mouseDoubleClick(QPointF);
    void mouseOver( QPointF);
    void pointSelected(QPointF);
    void lineSelected(QLineF);
    void polygonSelected(QPolygonF);

protected:

    virtual void refreshScene()=0;


    void mousePressEvent(QMouseEvent*)override;
    void mouseReleaseEvent(QMouseEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mouseDoubleClickEvent(QMouseEvent*)override;
    void keyPressEvent(QKeyEvent*)override;
    void leaveEvent(QEvent*)override;
    void resizeEvent(QResizeEvent*)override;
    void drawBackground(QPainter *painter, const QRectF &rect)override;
    void drawForeground(QPainter *painter, const QRectF &rect)override;

    QPointF toAxis(QPointF);
    QPointF toScene(QPointF);

private slots:
    void ticksChanged();
    void revertXAxis();
    void revertZAxis();
    void axisTypesChanged();
    void updateBackground();
    void updateSceneRectFromAxes();
    void updateViewRectFromAxes();
    void updateAxesRangeFromSceneRect();
    void updateAxesViewRangeFromViewRect();
    void addSelectionPolygonPoint( QPointF p );

private:

    Axis* m_xAxis=nullptr;
    Axis* m_zAxis=nullptr;
    bool m_xMesh=false;
    bool m_zMesh=false;
    bool m_zoomActive=false;
    bool m_selectionActive=false;
    QPointF m_selStart;
    ZOOMMODE m_zoomMode=ZOOMMODE::NONE;
    CURSORMODE m_cursorMode=CURSORMODE::NONE;
    SELECTIONMODE m_selectionMode=SELECTIONMODE::None;
    MouseMode m_mouseMode=MouseMode::Explore;

    QRubberBand* m_selectionRubberband=nullptr;
    QRubberBand* m_xCursorRubberband=nullptr;
    QRubberBand* m_zCursorRubberband=nullptr;

    QPoint              m_mouseSelectionStart;
    QPointF             m_selectedPoint;
    QLineF              m_selectedLine;         // axis coords
    QPolygonF           m_selectionPolygon;     // axis coords
};

#endif // TRACKVIEW_H
