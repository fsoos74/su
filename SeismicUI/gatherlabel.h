#ifndef GATHERLABEL_H
#define GATHERLABEL_H


#include<QLabel>
#include<QVector>
#include<QPainterPath>
#include<memory>
#include<QPixmap>

#include<QPoint>
#include "gather.h"
#include <grid2d.h>

class GatherView;
class ColorTable;

class GatherLabel : public QLabel{
friend class GatherView;
    Q_OBJECT

public:



    enum class TraceDisplayMode{ WigglesVariableArea, Density };

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

    size_t highlightedTrace()const{
        return m_highlightedTrace;
    }

    const QColor& horizonColor()const{
        return m_horizonColor;
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

public slots:

    void setTraceColor(QColor);
    void setTraceOpacity(int);
    void setDensityOpacity(int);
    void setVolumeOpacity(int);
    void setHorizonColor(QColor);
    void setDisplayDensity(bool);
    void setDisplayWiggles(bool);
    void setDisplayVariableArea(bool);
    void setDisplayLines(bool);
    void setHighlightTrace( bool on=true);
    void setHighlightedTrace( size_t);
    void updateTraceScaleFactors();
    void updateBuffers();

signals:

    void displayDensityChanged(bool);
    void displayWigglesChanged(bool);
    void displayVariableAreaChanged(bool);
    void displayLinesChanged(bool);
    void horizonColorChanged(QColor);

protected:

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private slots:

        void onViewGatherChanged( std::shared_ptr<seismic::Gather>);

private:

    void drawHorizons( QPainter& painter );
    void drawHorizon(QPainter& painter, std::shared_ptr<Grid2D<double>> g, QPen thePen);
    void drawHorizontalLines(QPainter& painter, const QRect& rect);
    void updateTimeRange();
    void updatePixmap();
    //void computePixelPerUnits();
    void updateTracePaths();
    void updateDensityPlot();
    void updateVolumePixmap();


    GatherView* m_view;

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

    QColor m_horizonColor=Qt::red;
    bool m_highlightTrace=false;
    size_t m_highlightedTrace=0;


    QVector<qreal> traceScaleFactors;
    QVector<QPainterPath> m_traceWigglePaths;
    QVector<QPainterPath> m_traceVariableAreaPaths;
    QPixmap m_densityPlot;
    QPixmap m_volumePixmap;
    QPixmap m_pixmap;
    QPoint m_pixmapTopLeft;
    bool m_dirty=true;               // this specifies whether the buffer was changed and not painted yet
};

#endif // GATHERLABEL_H
