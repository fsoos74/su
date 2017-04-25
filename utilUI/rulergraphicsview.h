#ifndef RULERGRAPHICSVIEW_H
#define RULERGRAPHICSVIEW_H

#include<QGraphicsView>
#include<QWidget>
#include<QRubberBand>
#include<QPolygonF>
#include<QPointF>
#include<mousemode.h>

#include<functional>

class RulerGraphicsView;

class GVRuler : public QWidget{

    Q_OBJECT

public:

    enum Orientation{ VERTICAL_RULER, HORIZONTAL_RULER };
    enum TickType{ MAIN_TICK, SUB_TICK };

    class Tick{

    public:
        Tick( int c=0, qreal val=0, TickType type=MAIN_TICK):m_coord(c), m_value(val), m_type(type) {
        }

        int coord()const{
            return m_coord;
        }

        TickType type()const{
            return m_type;
        }

        qreal value()const{
            return m_value;
        }

    private:
        int m_coord;
        qreal m_value;
        TickType m_type;
    };

    GVRuler( RulerGraphicsView* parent, Orientation orient );

    const QString& label()const{
        return m_label;
    }

    bool isAutoTickIncrement()const{
        return m_autoTickIncrement;
    }

    qreal tickIncrement()const{
        return m_tickIncrement;
    }

    int subTickCount()const{
        return m_subTickCount;
    }

    int tickMarkSize()const{
        return m_TICK_MARK_SIZE;
    }

    int subTickMarkSize()const{
        return m_subTickMarkSize;
    }

    int minimumPixelIncrement()const{
        return m_minimumPixelIncrement;
    }

    qreal minimumValueIncrement()const{
        return m_minimumValueIncrement;
    }

    QVector< Tick > computeTicks()const;

    void setAnnotationFunction( std::function<QString(qreal)> );

public slots:

    void setAutoTickIncrement(bool);
    void setTickIncrement(qreal);
    void setSubTickCount(int);
    void setTickMarkSize(int);
    void setSubTickMarkSize(int);
    void setMinimumValueIncrement(qreal);
    void setMinimumPixelIncrement(int);
    void setLabel( const QString&);

protected:


    void paintEvent( QPaintEvent*);


private:

    std::pair<qreal,int> computeTickIncrement(int size_pix, qreal size_data)const;  // also computes sub tick count


    void drawVertical( QPainter& painter );
    void drawHorizontal( QPainter& painter );

    RulerGraphicsView* m_view;
    Orientation m_orientation;
    QString m_label;
    std::function<QString(qreal)> m_annotationFunction=[](qreal value){return QString::number(value);};

    qreal   m_minimumValueIncrement=0;          // ticks will be at least spaced at this value distance
    int     m_minimumPixelIncrement=50;         // ticks will be at least that many pixels  apart
    qreal   m_tickIncrement=1;                  // current tick increment
    int     m_subTickCount=0;                   // number of subticks per tick
    bool    m_autoTickIncrement=true;           // automatically compute tick increment
    int     m_TICK_MARK_SIZE=5;
    int     m_subTickMarkSize=3;
    //int m_TICK_LABEL_DX=100;
    //int m_TICK_LABEL_DY=50;
};



class RulerGraphicsView : public QGraphicsView{

    Q_OBJECT

public:

    friend class GVRuler;

    enum class GridMode{ None, Background, Foreground };

    enum class SelectionMode{ None, SinglePoint, Line, Polygon };

    const int MINIMUM_ZOOM_PIXEL=6;

    RulerGraphicsView(QWidget* parent=nullptr);

    GVRuler* leftRuler()const{
        return m_leftRuler;
    }

    GVRuler* topRuler()const{
        return m_topRuler;
    }

    MouseMode mouseMode()const{
        return m_mouseMode;
    }

    GridMode gridMode()const{
        return m_gridMode;
    }

    int rulerWidth()const{
        return m_RULER_WIDTH;
    }

    int rulerHeight()const{
        return m_RULER_HEIGHT;
    }

    SelectionMode selectionMode()const{
        return m_selectionMode;
    }

    QPointF selectedPoint()const{
        return m_selectedPoint;
    }

    QPolygonF selectionPolygon()const{
        return m_selectionPolygon;
    }

    QPen gridPen()const{
        return m_gridPen;
    }

    QPen subGridPen()const{
        return m_subGridPen;
    }

    Qt::AspectRatioMode aspectRatioMode()const{
        return m_aspectRatioMode;
    }

public slots:

    void setGridMode( GridMode );
    void setMouseMode(MouseMode);
    void setRulerWidth( int );
    void setRulerHeight( int );
    void zoomFitWindow();
    void zoomBy( qreal factor );
    void setSelectionMode( SelectionMode );
    void setSelectionPolygon( QPolygonF );
    void setGridPen(QPen);
    void setSubGridPen(QPen);
    void setAspectRatioMode( Qt::AspectRatioMode);
signals:
    void mouseModeChanged(MouseMode);
    void mouseOver( QPointF);

protected:

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void resizeEvent( QResizeEvent*);
    bool eventFilter(QObject *, QEvent *);
    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

private:

    void addSelectionPolygonPoint( QPointF);
    void updateLayout();
    QPoint mouseEventPosToViewport( QPoint, bool );
    void drawGrid( QPainter* painter, const QRect& rectInPixel );


    GVRuler* m_leftRuler=nullptr;
    GVRuler* m_topRuler=nullptr;

    MouseMode m_mouseMode=MouseMode::Explore;
    GridMode m_gridMode=GridMode::Background;
    SelectionMode m_selectionMode=SelectionMode::None;

    int m_RULER_WIDTH=100;
    int m_RULER_HEIGHT=60;

    qreal m_zoomFactor=1;

    QPen m_gridPen=QPen(Qt::gray,0);
    QPen m_subGridPen=QPen(Qt::lightGray,0);

    Qt::AspectRatioMode m_aspectRatioMode = Qt::IgnoreAspectRatio;

    QRubberBand*        m_rubberBand=nullptr;
    bool                m_mouseSelection=false;
    QWidget*            m_mouseSelectionWidget=nullptr;
    QPoint              m_mouseSelectionStart;
    QPointF             m_selectedPoint;
    QPolygonF           m_selectionPolygon;
};

#endif // RULERGRAPHICSVIEW_H
