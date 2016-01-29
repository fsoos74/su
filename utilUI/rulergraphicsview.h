#ifndef RULERGRAPHICSVIEW_H
#define RULERGRAPHICSVIEW_H

#include<QGraphicsView>
#include<QWidget>
#include<QRubberBand>
#include<QPolygonF>
#include<QPointF>

#include<functional>

class RulerGraphicsView;

class GVRuler : public QWidget{

    Q_OBJECT

public:

    enum Orientation{ VERTICAL_RULER, HORIZONTAL_RULER };

    class Tick{

    public:
        Tick( int c=0, qreal val=0):m_coord(c), m_value(val){
        }

        int coord()const{
            return m_coord;
        }

        qreal value()const{
            return m_value;
        }

    private:
        int m_coord;
        qreal m_value;
    };

    GVRuler( RulerGraphicsView* parent, Orientation orient );

    const QString& label()const{
        return m_label;
    }

    QVector< Tick > computeTicks()const;

    void setAnnotationFunction( std::function<QString(qreal)> );

public slots:

    void setTickMarkSize(int);
    void setMinimumValueIncrement(qreal);
    void setMinimumPixelIncrement(int);
    void setLabel( const QString&);

protected:


    void paintEvent( QPaintEvent*);


private:

    qreal tickIncrement(int size_pix, qreal size_data)const;


    void drawVertical( QPainter& painter );
    void drawHorizontal( QPainter& painter );

    RulerGraphicsView* m_view;
    Orientation m_orientation;
    QString m_label;
    std::function<QString(qreal)> m_annotationFunction=[](qreal value){return QString::number(value);};

    qreal   m_minimumValueIncrement=0;          // ticks will be at least spaced at this value distance
    int     m_minimumPixelIncrement=50;         // ticks will be at least that many pixels  apart
    int m_TICK_MARK_SIZE=5;
    //int m_TICK_LABEL_DX=100;
    //int m_TICK_LABEL_DY=50;
};



class RulerGraphicsView : public QGraphicsView{

    Q_OBJECT

public:

    friend class GVRuler;

    enum class GridMode{ None, Background, Foreground };

    enum class SelectionMode{ None, SinglePoint, Polygon };

    const int MINIMUM_ZOOM_PIXEL=6;

    RulerGraphicsView(QWidget* parent=nullptr);

    GVRuler* leftRuler()const{
        return m_leftRuler;
    }

    GVRuler* topRuler()const{
        return m_topRuler;
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

public slots:

    void setGridMode( GridMode );
    void setRulerWidth( int );
    void setRulerHeight( int );
    void zoomFitWindow();
    void zoomBy( qreal factor );
    void setSelectionMode( SelectionMode );
    void setSelectionPolygon( QPolygonF );

signals:
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

    GridMode m_gridMode=GridMode::Background;
    SelectionMode m_selectionMode=SelectionMode::None;

    int m_RULER_WIDTH=100;
    int m_RULER_HEIGHT=60;

    qreal m_zoomFactor=1;

    QRubberBand*        m_rubberBand=nullptr;
    bool                m_mouseSelection=false;
    QWidget*            m_mouseSelectionWidget=nullptr;
    QPoint              m_mouseSelectionStart;
    QPointF             m_selectedPoint;
    QPolygonF           m_selectionPolygon;
};

#endif // RULERGRAPHICSVIEW_H
