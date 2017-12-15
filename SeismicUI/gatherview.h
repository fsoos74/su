#ifndef GATHERVIEW_H
#define GATHERVIEW_H


#include<QScrollArea>
#include<QRubberBand>
#include<QLabel>
#include<memory>
#include<QVector>
#include<QMap>
#include<functional>
#include<gather.h>
#include<gatherscaler.h>
#include<volume.h>
#include <mousemode.h>

#include "gathersortkey.h"

class GatherLabel;
class GatherRuler;
class AxxisLabelWidget;

#include<selectionpoint.h>

#include<grid2d.h>

#include <picker.h>


class GatherView : public QScrollArea
{
    Q_OBJECT

public:


    const int RULER_WIDTH=100;
    const int RULER_HEIGHT=80;

    typedef std::function< QStringList( const seismic::Header&) > TraceAnnotationFunction;

    GatherView( QWidget* parent=nullptr);

    const QVector<SelectionPoint>& highlightedPoints()const{
        return m_highlightedPoints;
    }

    const SelectionPoint cursorPosition()const{
        return m_cursorPosition;
    }

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    std::shared_ptr<Grid2D<float> > horizon(QString name)const;

    QStringList horizonList();

    QColor horizonColor(QString);

    std::shared_ptr< Volume > volume()const;

    Picker* picker()const{
        return m_picker;
    }

    GatherLabel* gatherLabel()const{
        return m_gatherLabel;
    }

    GatherRuler* leftRuler()const{
        return m_leftRuler;
    }

    GatherRuler* topRuler()const{
        return m_topRuler;
    }

    MouseMode mouseMode()const{
        return m_mouseMode;
    }

    AxxisLabelWidget* axxisLabelWidget()const{
        return m_axxisLabelWidget;
    }

    GatherScaler* gatherScaler()const{
        return m_gatherScaler;
    }

    const std::vector<std::string>& annotationKeys()const{
        return m_annotationKeys;
    }

    qreal pixelPerTrace()const{
        return m_pixelPerTrace;
    }

    qreal pixelPerSecond()const{
        return m_pixelPerSecond;
    }

    bool isFixedScale()const{
        return m_fixedScale;
    }

    qreal ft()const{
        return m_ft;
    }

    qreal lt()const{
        return m_lt;
    }

    qreal seconds()const{
        return (m_ft<m_lt) ? m_lt - m_ft : 0;
    }

    QStringList traceAnnotation( size_t traceNumber )const;

    GatherSortKey primarySortKey()const{
        return m_primarySortKey;
    }

     int lookupTrace(int iline, int xline);  // return -1 if not found

     QVector<int> intersectionTraces()const{
         return m_intersectionTraces;
     }

     QVector<qreal> intersectionTimes()const{
         return m_intersectionTimes;
     }

signals:

    void gatherChanged( std::shared_ptr<seismic::Gather>);
    void primarySortKeyChanged(GatherSortKey);
    void pixelPerTraceChanged(qreal);
    void pixelPerSecondChanged(qreal);
    void fixedScaleChanged(bool);
    void traceAnnotationFuntionChanged();
    void mouseOver( int traceNo, qreal second);
    void topRulerClicked( int traceNo );
    void leftRulerClicked( qreal secs );
    void traceClicked( size_t );
    //void traceSelected( size_t );
    void pointSelected( SelectionPoint );
    void mouseModeChanged( MouseMode);

public slots:

    void setGather( std::shared_ptr<seismic::Gather>);
    void setPrimarySortKey(GatherSortKey);
    void setIntersectionTraces( QVector<int>);
    void setIntersectionTimes( QVector<qreal>);
    void setHighlightedPoints(QVector<SelectionPoint>);
    void setCursorPosition(SelectionPoint);
    void setMouseMode( MouseMode );
    void addHorizon( QString name, std::shared_ptr<Grid2D<float> > g, QColor);
    //void setHorizonColor( QString name, QColor);
    void removeHorizon( QString name);
    //void setPicks( std::shared_ptr<Grid2D<float>>);
    void setVolume( std::shared_ptr<Volume>);    // set null for closing
    void setAnnotationKeys( const std::vector<std::string>&);
    void setPixelPerTrace( qreal );
    void setPixelPerSecond( qreal );
    void setPixelPerUnits( qreal pixPerTrace, qreal pixPerSecond);
    void setFixedScale(bool);
    void normalize();
    void zoom(QRect);
    void zoomBy(qreal);


protected:

    void resizeEvent(QResizeEvent*);
    bool eventFilter(QObject *, QEvent *);

private:

    bool eventFilterExplore(QWidget* widget, QMouseEvent *mouseEvent);
    bool eventFilterZoom(QWidget* widget, QMouseEvent *mouseEvent);
    bool eventFilterSelect(QWidget* widget, QMouseEvent *mouseEvent);
    bool eventFilterPick(QWidget* widget, QMouseEvent *mouseEvent);
    bool eventFilterDeletePick(QWidget* widget, QMouseEvent *mouseEvent);

    void buildTraceLookup();

    QPoint mouseEventToLabel( QPoint, bool start=true );    // if start ruler val set to 0 else to rightmost/bottommost
    void updateLayout();
    void updateTimeRange();
    void adjustScrollBar(QScrollBar *scrollBar, qreal factor);

    //void buildGatherIndex();

    std::shared_ptr<seismic::Gather> m_gather;

    GatherSortKey m_primarySortKey=GatherSortKey::None;

    QMap<QString, std::shared_ptr<Grid2D<float> > > m_horizons;
    QMap<QString, QColor> m_horizonColors;

    std::shared_ptr<Volume> m_volume;

    Picker* m_picker=nullptr;

    QMap< QString, int> m_traceLookup;       // key is combination il and xl : "iline_xline"

    QVector<int>    m_intersectionTraces;
    QVector<qreal>  m_intersectionTimes;
    QVector<SelectionPoint> m_highlightedPoints;
    SelectionPoint m_cursorPosition;

    GatherLabel* m_gatherLabel=nullptr;
    GatherRuler* m_leftRuler;
    GatherRuler* m_topRuler;
    AxxisLabelWidget* m_axxisLabelWidget;
    GatherScaler* m_gatherScaler;
    std::vector<std::string> m_annotationKeys;

    qreal m_pixelPerTrace=50;
    qreal m_pixelPerSecond=500;
    bool  m_fixedScale=false;           // whether zooming/resizing of content(label) is possible

    qreal m_ft=0;
    qreal m_lt=0;

    QRubberBand*        rubberBand=nullptr;
    bool                mouseSelection=false;
    QWidget*            mouseSelectionWidget=nullptr;
    QPoint              mouseSelectionStart;

   MouseMode            m_mouseMode;
};




#endif // GATHERVIEW_H
