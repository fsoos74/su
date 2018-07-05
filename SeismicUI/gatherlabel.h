#ifndef GATHERLABEL_H
#define GATHERLABEL_H


#include<QLabel>
#include<QVector>
#include<QPainterPath>
#include<memory>
#include<QPixmap>
#include<QMap>

#include<QPoint>
#include "gather.h"
#include <grid2d.h>
#include <table.h>

#include<selectionpoint.h>

class GatherView;
class ColorTable;

class GatherLabel : public QLabel{
friend class GatherView;
    Q_OBJECT

public:



    enum class TraceDisplayMode{ WigglesVariableArea, Density };

    struct PointDisplayOptions{
        int size;
        QColor color;
        int opacity;
        bool operator==(const PointDisplayOptions& other)const{
            return size==other.size && color==other.color && opacity==other.opacity;
        }
    };

    struct LineDisplayOptions{
        int width;
        QColor color;
        int opacity;
        bool operator==(const LineDisplayOptions& other)const{
            return width==other.width && color==other.color && opacity==other.opacity;
        }
    };

    GatherLabel( GatherView* parent);

    GatherView* view()const{
        return m_view;
    }

    bool isDisplayDensity()const{
        return m_density;
    }

    bool isDisplayWiggles()const{
        return m_wiggles;
    }

    bool isDisplayVariableArea()const{
        return m_variableArea;
    }

    bool displayLines()const{
        return m_displayLines;
    }

    int highlightedTrace()const{
        return m_highlightedTrace;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    ColorTable* volumeColorTable()const{
        return m_volumeColorTable;
    }

    QColor traceColor()const{
        return m_traceColor;
    }

    int traceOpacity()const{
        return m_traceOpacity;
    }

    int densityOpacity()const{
        return m_densityOpacity;
    }

    int volumeOpacity()const{
        return m_volumeOpacity;
    }

    PointDisplayOptions pickDisplayOptions()const{
        return m_pickDisplayOptions;
    }

    int pickSize()const{
        return m_pickDisplayOptions.size;
    }

    QColor pickColor()const{
        return m_pickDisplayOptions.color;
    }

    int pickOpacity()const{
        return m_pickDisplayOptions.opacity;
    }

    PointDisplayOptions highlightedPointDisplayOptions()const{
        return m_highlightedPointDisplayOptions;
    }

    int highlightedPointSize()const{
        return m_highlightedPointDisplayOptions.size;
    }

    QColor highlightedPointColor()const{
        return m_highlightedPointDisplayOptions.color;
    }

    int highlightedPointOpacity()const{
        return m_highlightedPointDisplayOptions.opacity;
    }


    LineDisplayOptions horizonDisplayOptions(const QString& name)const{
        return m_horizonDisplayOptions.value(name, LineDisplayOptions{1,Qt::black,100});
    }

    int horizonWidth(const QString& name)const{
        return horizonDisplayOptions(name).width;
    }

    QColor horizonColor(const QString& name)const{
        return horizonDisplayOptions(name).color;
    }

    int horizonOpacity(const QString& name)const{
        return horizonDisplayOptions(name).opacity;
    }

    int viewerCurrentTrace()const{
        return m_viewerCurrentTrace;
    }

public slots:

    void setTraceColor(QColor);
    void setTraceOpacity(int);
    void setDensityOpacity(int);

    void setVolumeOpacity(int);

    void setPickDisplayOptions(PointDisplayOptions);
    void setPickSize(int);
    void setPickColor(QColor);
    void setPickOpacity(int);

    void setHighlightedPointDisplayOptions(PointDisplayOptions);
    void setHighlightedPointSize(int);
    void setHighlightedPointColor(QColor);
    void setHighlightedPointOpacity(int);

    void removeHorizonDisplayOptions(QString);
    void setHorizonDisplayOptions( QString, LineDisplayOptions);
    void setHorizonWidth(QString,int);
    void setHorizonColor(QString,QColor);
    void setHorizonOpacity(QString,int);

    void setDisplayDensity(bool);
    void setDisplayWiggles(bool);
    void setDisplayVariableArea(bool);
    void setDisplayLines(bool);
    void setHighlightTrace( bool on=true);
    void setHighlightedTrace( int );
    void setViewerCurrentTrace(int);
    void updateTraceScaleFactors();
    void updateBuffers();

signals:

    void displayDensityChanged(bool);
    void displayWigglesChanged(bool);
    void displayVariableAreaChanged(bool);
    void displayLinesChanged(bool);

protected:

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private slots:

        void onViewGatherChanged( std::shared_ptr<seismic::Gather>);

private:

    void drawHorizons( QPainter& painter );
    void drawHorizon(QPainter& painter, std::shared_ptr<Grid2D<float>> g, LineDisplayOptions);
    void drawPicks(QPainter& painter, Table* picks);
    void drawHorizontalLines(QPainter& painter, const QRect& rect);
    void drawHighlightedPoints( QPainter& painter, const QVector<SelectionPoint>& points);
    void drawViewerCurrentPosition( QPainter& painter, SelectionPoint);
    void drawIntersectionTraces( QPainter& painter, const QVector<int>& trc);
    void drawIntersectionTimes( QPainter& painter, const QVector<qreal>& times);
    void drawPickerBuffer( QPainter& painter, std::vector<std::pair<int,float>> points);
    void updateTimeRange();
    void updatePixmap();
    //void computePixelPerUnits();
    void updateTracePaths();
    void updateDensityPlot();
    void updateVolumePixmap();

    // this is now part of gatherview
    //void buildTraceLookup();
    //int lookupTrace( int iline, int xline );

    GatherView* m_view=nullptr;

    bool m_density=false;
    bool m_wiggles=true;
    bool m_variableArea=true;

    bool m_displayLines=true;

    ColorTable* m_colorTable=nullptr;
    ColorTable* m_volumeColorTable=nullptr;
    QColor m_traceColor=Qt::black;
    int m_traceOpacity=100;
    int m_densityOpacity=100;
    int m_volumeOpacity=50;

    PointDisplayOptions m_pickDisplayOptions=PointDisplayOptions{4,Qt::blue,50};
    PointDisplayOptions m_highlightedPointDisplayOptions=PointDisplayOptions{4,Qt::red,100};
    QMap<QString, LineDisplayOptions> m_horizonDisplayOptions;

    bool m_highlightTrace=false;
    int m_highlightedTrace=-1;
    int m_viewerCurrentTrace=-1;

    QMap<int,int> traceLookup;
    QVector<qreal> traceScaleFactors;
    qreal bias=0;
    QVector<QPainterPath> m_traceWigglePaths;
    QVector<QPainterPath> m_traceVariableAreaPaths;
    QPixmap m_densityPlot;
    QPixmap m_volumePixmap;
    QPixmap m_pixmap;                // background pixmap of actual visible content
    QPoint m_pixmapTopLeft;          // top left coords (pixel) of content in background pixmap
    bool m_dirty=true;               // this specifies whether the buffer was changed and not painted yet

    //QPixmap m_densityPlotFS;    // full size background pixmap of density plot
    //QPixmap m_volumePlotFS;     // full size background pixmap for volume, covers only actual volume
};

#endif // GATHERLABEL_H
