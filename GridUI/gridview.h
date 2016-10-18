#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include<QScrollArea>
#include<QLabel>
#include<QVector>
#include<QLineF>
#include<QRgb>
#include<QRubberBand>
#include<memory>

#include<grid2d.h>
#include<colortable.h>

#include "axxisorientation.h"

#include <mousemode.h>



//#include<projectdispatcher.h>

#include <selectionpoint.h>

class  GridView;
class GridViewer;


class Ruler : public QWidget{

    Q_OBJECT

public:

    enum Orientation{ VERTICAL_RULER, HORIZONTAL_RULER };

    class Tick{

    public:
        Tick( int c=0, int val=0):m_coord(c), m_value(val){
        }

        int coord()const{
            return m_coord;
        }

        int value()const{
            return m_value;
        }

    private:
        int m_coord;
        int m_value;
    };

    const int TICK_MARK_SIZE=5;
    const int TICK_LABEL_DX=60;
    const int TICK_LABEL_DY=60;

    Ruler( GridView* parent, Orientation orient );

    int tickIncrement()const{
        return m_tickIncrement;
    }

    bool isFixedTickIncrement()const{
        return m_fixedTickIncrement;
    }

    qreal cursorPosition()const{
        return m_currentPos;
    }

    QVector< Tick > computeTicks( int coord1, int coord2 )const;

signals:

    void tickIncrementChanged(int);
    void fixedTickIncrementChanged(bool);
    void ticksChanged();

public slots:

    void setFixedTickIncrement(bool);
    void setTickIncrement(int);
    void setCurrentPos(qreal);

private slots:

    void updateTickIncrement();

protected:
    void paintEvent( QPaintEvent*);
    void resizeEvent( QResizeEvent* );

private:

    int autoTickIncrement(int size_pix, int size_grid, int min_distance)const;

    void drawVertical( QPainter& painter );
    void drawHorizontal( QPainter& painter );
    void drawHorizontalIndicator( QPainter&, QPoint );
    void drawVerticalIndicator( QPainter&, QPoint );

    GridView* m_view;
    qreal m_currentPos;
    Orientation m_orientation;
    int m_tickIncrement=1;
    bool m_fixedTickIncrement=false;
};


class ViewLabel : public QLabel{

Q_OBJECT

public:

    ViewLabel( GridView* parent);

    QImage image()const{
        return m_image;
    }

    Qt::TransformationMode transformationMode()const{
        return m_transformationMode;
    }


    const QVector<QLineF>& isoLineSegments()const{
        return m_isoLineSegments;
    }

    const QColor& isoLineColor()const{
        return m_isoLineColor;
    }

    int isoLineWidth()const{
        return m_isoLineWidth;
    }

    const QColor& highlightCDPColor()const{

        return m_highlightCDPColor;
    }

    int highlightCDPSize()const{

        return m_highlightCDPSize;
    }

    bool showViewerCurrentPoint()const{
        return m_showViewerCurrentPoint;
    }

public slots:

    void setImage( QImage );

    void setIsoLineColor( const QColor& );

    void setIsoLineWidth( int );

    void setHighlightCDPSize( int );

    void setHighlightCDPColor( QColor );

    void setShowViewerCurrentPoint(bool);

    void setIsoLineSegments( const QVector<QLineF>& );

    void setTransformationMode( Qt::TransformationMode );

    void updateLineSegments();

signals:

    void imageChanged();

    void isoLineValuesChanged( const QVector<double>& );

    void isoLineSegmentsChanged( const QVector<QLineF>&);

    void transformationModeChanged( Qt::TransformationMode);

    void isoLineColorChanged( QColor);

    void isoLineWidthChanged(int);

    void highlightCDPColorChanged( QColor);

    void highlightCDPSizeChanged(int);

    void showViewerCurrentPointChanged(bool);

//    void mouseOver(int, int);

protected:

    void paintEvent( QPaintEvent*);
    void resizeEvent( QResizeEvent*);
    //void mouseMoveEvent( QMouseEvent* );

private:

   void updatePixmap();
   void computeIsoLines();
   void drawVerticalGrid( QPainter& painter);
   void drawHorizontalGrid( QPainter& painter);
   void drawHighlightedCDPs( QPainter& painter);
   void drawIntersectionPoints( QPainter& painter);
   void drawViewerCurrentPoint( QPainter& painter);
   void drawPolyline( QPainter&, const QPen& );

   GridView* m_view;
   QImage m_image;
   QVector<QLineF> m_isoLineSegments;
   QVector<QLineF> paintSegs;
   Qt::TransformationMode m_transformationMode=Qt::FastTransformation;
   QColor m_isoLineColor;
   int m_isoLineWidth;
   int m_highlightCDPSize=4;
   QColor m_highlightCDPColor=Qt::red;
   bool m_showViewerCurrentPoint=true;
};


class GridView : public QScrollArea
{
friend class Ruler;
friend class ViewLabel;

Q_OBJECT

public:

    //enum class AxxisOrientation{ Horizontal, Vertical };
    //enum class AxxisDirection{ Ascending, Descending };

    const int RULER_WIDTH=100;
    const int RULER_HEIGHT=60;

    const int MAX_COLORS=256;


    GridView(QWidget* parent=nullptr);

    ~GridView();

    //ProjectDispatcher* dispatcher() const;

    ViewLabel* viewLabel()const{
        return m_label;
    }

    std::shared_ptr<Grid2D<float>> grid()const{
        return m_grid;                   //!!!!
    }

    bool isColorMappingLocked(){
        return m_colorMappingLock;
    }

    bool isFixedAspectRatio()const{
        return m_fixedAspectRatio;
    }

    qreal aspectRatio()const{
        return m_aspectRatio;
    }

    const QVector<SelectionPoint>& highlightedCDPs()const{
        return m_highlightedCDPs;
    }

    const QVector<SelectionPoint>& intersectionPoints()const{
        return m_intersectionPoints;
    }

    const SelectionPoint cursorPosition()const{
        return m_cursorPosition;
    }

    const QVector<QPoint>& polylineSelection()const{
        return m_polyline;
    }




    ColorTable* colorTable()const{
        return m_colorTable;
    }

    QColor nullColor()const{
        return m_nullColor;
    }


    const QString& inlineAxxisLabel()const{
        return m_inlineAxxisLabel;
    }

    const QString& crosslineAxxisLabel()const{
        return m_crosslineAxxisLabel;
    }

    const QTransform& gridToImageTransform()const{
        return m_gridToImageTransform;
    }

    const QTransform& imageToGridTransform()const{
        return m_imageToGridTransform;
    }

    AxxisOrientation inlineOrientation()const{
        return m_inlineOrientation;
    }

    AxxisDirection inlineDirection()const{
        return m_inlineDirection;
    }

    AxxisDirection crosslineDirection()const{
        return m_crosslineDirection;
    }

    const QVector<double>& isoLineValues()const{
        return m_isoLineValues;
    }

    Ruler* horizontalRuler(){
        return m_topRuler;
    }

    Ruler* verticalRuler(){
        return m_leftRuler;
    }

     ViewLabel* label()const{ return m_label;}

     MouseMode mouseMode()const{
         return m_mouseMode;
     }

     qreal computeILXLBasedAspectRatio()const;

signals:

    void gridChanged( std::shared_ptr<Grid2D<float> >);
    void fixedAspectRatioChanged(bool);
    void aspectRatioChanged(qreal);
    void colorTableChanged( ColorTable*);
    void nullColorChanged( QColor );
    void scaleChanged(double);
    void gridToImageTransformChanged( QTransform );

    void inlineOrientationChanged( AxxisOrientation );
    void inlineDirectionChanged( AxxisDirection );
    void crosslineDirectionChanged( AxxisDirection);

    void inlineAxxisLabelChanged( const QString&);
    void crosslineAxxisLabelChanged( const QString&);

    void mouseOver( int iline, int xline );

public slots:

    void setGrid( std::shared_ptr<Grid2D<float> > );
    void setFixedAspectRatio(bool);
    void setAspectRatio(qreal);

    void setHighlightedCDPs( QVector<SelectionPoint>);
    void setViewerCurrentPoint(SelectionPoint);
    void setIntersectionPoints( QVector<SelectionPoint>);
    void setColorMapping( const std::pair<double,double>& );
    void setColorMappingLocked(bool);
    void setColors( const QVector<QRgb>&);
    void setNullColor( QColor );
    void setGridToImageTransform( const QTransform&);

    void setIsoLineValues(const QVector<double>&);
    void setInlineOrientation( AxxisOrientation);
    void setInlineDirection( AxxisDirection );
    void setCrosslineDirection( AxxisDirection );
    void setOrientationParams( AxxisOrientation inlineOrientation, AxxisDirection inlineDirection, AxxisDirection crosslineDirection);

    void setInlineAxxisLabel( const QString& );
    void setCrosslineAxxisLabel( const QString& );

     void zoom( QRect rect );
     void zoomBy( qreal );
     void zoomFit();

     void setMouseMode(MouseMode);

signals:

     void pointSelected(SelectionPoint);
     void polylineSelected(QVector<QPoint>);
     void mouseModeChanged(MouseMode);

protected:

    void resizeEvent( QResizeEvent*);
    bool eventFilter(QObject *, QEvent *);
    void keyPressEvent(QKeyEvent*);

private slots:


    void onColorTableChanged();

private:

    QPoint mouseEventToLabel( QPoint pt, bool start=true ); // if start orthogonal label values set to 0 else rightmost/bottommost
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateLayout();
    QImage grid2image();
    QTransform computeGridToImageTransform()const;
    QTransform computeGrid2ImageTransform_IlHorizontal()const;
    QTransform computeGrid2ImageTransform_IlVertical()const;

    QSize adjustedToAspectRatio(QSize, bool grow=false)const;


    GridViewer* m_viewer=nullptr;
    ViewLabel* m_label=nullptr;
    Ruler* m_leftRuler=nullptr;
    Ruler* m_topRuler=nullptr;
    std::shared_ptr<Grid2D<float> > m_grid;
    bool m_fixedAspectRatio=true;
    qreal m_aspectRatio=1.; // w/h
    ColorTable* m_colorTable;           // holds colors and display range
    bool m_colorMappingLock=false;
    QColor m_nullColor;
    QTransform m_gridToImageTransform;
    QTransform m_imageToGridTransform;
     QVector<double> m_isoLineValues;
    AxxisOrientation m_inlineOrientation=AxxisOrientation::Horizontal;
    AxxisDirection m_inlineDirection=AxxisDirection::Ascending;
    AxxisDirection m_crosslineDirection=AxxisDirection::Ascending;
    QString m_inlineAxxisLabel="Inline Number";
    QString m_crosslineAxxisLabel="Crossline Number";

    QVector<SelectionPoint> m_highlightedCDPs;
    QVector<SelectionPoint>m_intersectionPoints;
    SelectionPoint m_cursorPosition;
    QVector<QPoint> m_polyline;

    QRubberBand*        rubberBand=nullptr;
    bool                mouseSelection=false;
    QWidget*            mouseSelectionWidget=nullptr;
    QPoint              mouseSelectionStart;

    MouseMode m_mouseMode=MouseMode::Explore;
};



#endif // GRIDVIEW_H
