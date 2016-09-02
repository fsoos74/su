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
#include<grid3d.h>

class GatherLabel;
class GatherRuler;
class AxxisLabelWidget;

#include<selectionpoint.h>

#include<grid2d.h>

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

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    std::shared_ptr<Grid2D<float> > horizon(QString name)const;

    QStringList horizonList();

    QColor horizonColor(QString);

    std::shared_ptr< Grid3D<float> > volume()const;

    GatherLabel* gatherLabel()const{
        return m_gatherLabel;
    }

    GatherRuler* leftRuler()const{
        return m_leftRuler;
    }

    GatherRuler* topRuler()const{
        return m_topRuler;
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

signals:

    void gatherChanged( std::shared_ptr<seismic::Gather>);
    void pixelPerTraceChanged(qreal);
    void pixelPerSecondChanged(qreal);
    void traceAnnotationFuntionChanged();
    void mouseOver( int traceNo, qreal second);
    void traceClicked( size_t );
    void traceSelected( size_t );

public slots:

    void setGather( std::shared_ptr<seismic::Gather>);
    void setHighlightedPoints(QVector<SelectionPoint>);
    void addHorizon( QString name, std::shared_ptr<Grid2D<float> > g, QColor);
    //void setHorizonColor( QString name, QColor);
    void removeHorizon( QString name);
    void setVolume( std::shared_ptr<Grid3D<float>>);    // set null for closing
    void setAnnotationKeys( const std::vector<std::string>&);
    void setPixelPerTrace( qreal );
    void setPixelPerSecond( qreal );
    void setPixelPerUnits( qreal pixPerTrace, qreal pixPerSecond);
    void normalize();
    void zoom(QRect);
    void zoomBy(qreal);

protected:

    void resizeEvent(QResizeEvent*);
    bool eventFilter(QObject *, QEvent *);

private:

    QPoint mouseEventToLabel( QPoint, bool start=true );    // if start ruler val set to 0 else to rightmost/bottommost
    void updateLayout();
    void updateTimeRange();
    void adjustScrollBar(QScrollBar *scrollBar, qreal factor);

    void buildGatherIndex();

    std::shared_ptr<seismic::Gather> m_gather;

    QMap<QString, std::shared_ptr<Grid2D<float> > > m_horizons;
    QMap<QString, QColor> m_horizonColors;


    std::shared_ptr<Grid3D<float>> m_volume;

    QVector<SelectionPoint> m_highlightedPoints;

    GatherLabel* m_gatherLabel=nullptr;
    GatherRuler* m_leftRuler;
    GatherRuler* m_topRuler;
    AxxisLabelWidget* m_axxisLabelWidget;
    GatherScaler* m_gatherScaler;
    std::vector<std::string> m_annotationKeys;

    qreal m_pixelPerTrace=50;
    qreal m_pixelPerSecond=500;

    qreal m_ft=0;
    qreal m_lt=0;

    QRubberBand*        rubberBand=nullptr;
    bool                mouseSelection=false;
    QWidget*            mouseSelectionWidget=nullptr;
    QPoint              mouseSelectionStart;

};




#endif // GATHERVIEW_H
