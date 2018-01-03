#ifndef CROSSPLOTVIEW_H
#define CROSSPLOTVIEW_H

#include <ruleraxisview.h>
#include <crossplot.h>
#include <volumedimensions.h>
#include <colortable.h>
#include<range.h>

class CrossplotView : public RulerAxisView
{
    Q_OBJECT

public:

    static const int DATA_INDEX_KEY=1;

    CrossplotView(QWidget* parent=nullptr);

    crossplot::Data data()const{
        return m_data;
    }

    VolumeDimensions region()const{
        return m_region;
    }

    QPointF trend()const{
        return m_trend;
    }

    bool isFlattenTrend()const{
        return m_flattenTrend;
    }

    bool isDisplayTrendLine()const{
        return m_displayTrendLine;
    }

    bool isFixedColor()const{
        return m_fixedColor;
    }

    int datapointSize()const{
        return m_datapointSize;
    }

    QColor pointColor()const{
        return m_pointColor;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    QColor trendlineColor()const{
        return m_trendlineColor;
    }

public slots:

    void setData(crossplot::Data);
    void setRegion(VolumeDimensions);

     void setTrend( QPointF ); // p(intercept, gradient)
     void setFlattenTrend(bool);
     void setDisplayTrendLine(bool);
     void setDatapointSize( int );
     void setFixedColor(bool);
     void setPointColor(QColor);
     void setColorMapping( const std::pair<double,double>& m);
     void setColors( const QVector<QRgb>&);
     void setTrendlineColor(QColor);

signals:
     void selectionChanged();

protected:
    void refreshScene()override;

private slots:
    void refresh();

private:
    void scanBounds();
    void scanData();

    crossplot::Data m_data;
    VolumeDimensions m_region;
    Range<float> m_xRange;
    Range<float> m_yRange;
    Range<float> m_attributeRange;

    bool m_displayTrendLine=false;
    bool m_flattenTrend=false;
    bool        m_fixedColor=true;      // points drawn with point color or colortable based on attribute
    QColor      m_pointColor=Qt::blue;  // datapoints are drawn with this color if fixed
    ColorTable* m_colorTable;           // holds colors and display range
    int m_datapointSize=11;
    QColor m_trendlineColor=Qt::red;
    QPointF m_trend;
};

#endif // CROSSPLOTVIEW_H
