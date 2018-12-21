#ifndef CROSSPLOTVIEW_H
#define CROSSPLOTVIEW_H

#include <ruleraxisview.h>
#include <crossplot.h>
#include <colortable.h>
#include<range.h>
#include<volumedimensions.h>
#include<limits>


class CrossplotView : public RulerAxisView
{
    Q_OBJECT

public:

    enum class Symbol{ Circle, Square, Cross};
    enum class ColorStyle{ Fixed, Attribute, Dataset};

    struct Filter{

        int minIL, maxIL;
        int minXL, maxXL;
        int minZ, maxZ;
        double minA, maxA;
        int minDS, maxDS;

        bool isInside(int il,int xl, int z, double a, int ds){
            return (il>=minIL) && (il<=maxIL) &&
                    (xl>=minXL) && (xl<=maxXL) &&
                    (z>=minZ) && (z<=maxZ) &&
                    (a>=minA) && (a<=maxA) &&
                    (ds>=minDS) && (ds<=maxDS);
        }
    };

    static const int DATA_INDEX_KEY=1;

    CrossplotView(QWidget* parent=nullptr);

    crossplot::Data data()const{
        return m_data;
    }

    VolumeDimensions dimensions()const{
        return m_dims;
    }

    Range<float> xRange()const{
        return m_xRange;
    }

    Range<float> yRange()const{
        return m_yRange;
    }

    Range<float> attributeRange()const{
        return m_attributeRange;
    }

    Filter filter()const{
        return m_filter;
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

    int pointSize()const{
        return m_pointSize;
    }

    QColor pointColor()const{
        return m_pointColor;
    }

    ColorStyle colorStyle()const{
        return m_colorStyle;
    }

    Symbol pointSymbol()const{
        return m_pointSymbol;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    QColor trendlineColor()const{
        return m_trendlineColor;
    }

public slots:

    void setData(crossplot::Data);
    void setFilter(Filter);

     void setTrend( QPointF ); // p(intercept, gradient)
     void setFlattenTrend(bool);
     void setDisplayTrendLine(bool);
     void setPointSymbol(CrossplotView::Symbol);
     void setColorStyle(CrossplotView::ColorStyle);
     void setPointSize( int );
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
    VolumeDimensions m_dims;
    Filter m_filter;
    Range<float> m_xRange;
    Range<float> m_yRange;
    Range<float> m_attributeRange;

    bool m_displayTrendLine=false;
    bool m_flattenTrend=false;

    Symbol      m_pointSymbol=Symbol::Circle;   // symbol used for points
    ColorStyle  m_colorStyle=ColorStyle::Fixed; // how point colors are assigned
    QColor      m_pointColor=Qt::blue;          // datapoints are drawn with this color if fixed
    ColorTable* m_colorTable;                   // holds colors and display range
    int m_pointSize=11;
    QColor m_trendlineColor=Qt::red;
    QPointF m_trend;
};


QString toQString(CrossplotView::Symbol);
CrossplotView::Symbol toSymbol(QString);

QString toQString(CrossplotView::ColorStyle);
CrossplotView::ColorStyle toColorStyle(QString);

#endif // CROSSPLOTVIEW_H
