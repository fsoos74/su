#ifndef SCATTERPLOTVIEW_H
#define SCATTERPLOTVIEW_H

#include <ruleraxisview.h>
#include <range.h>
#include <scatterplotpoint.h>

#include<QSet>
#include<QMap>

class ScatterplotView : public RulerAxisView
{
    Q_OBJECT

public:

    static const int DATA_INDEX_KEY=1;

    ScatterplotView(QWidget* parent=nullptr);

    int pointSize()const{
        return m_pointSize;
    }

    bool pointOutline()const{
        return m_pointOutline;
    }

    QColor pointColor(const QString&)const;

    const QVector<ScatterPlotPoint>& data()const{
        return m_data;
    }

    QStringList keys()const;
    QStringList displayedKeys()const;

public slots:
    void setPointSize(int);
    void setPointOutline(bool);
    void setPointColor(QString, QColor);
    void setDisplayedKeys( QStringList );
    void setData(QVector<ScatterPlotPoint>);

protected:
    void refreshScene()override;

private slots:
    void refresh();

private:
    void scanData();

    int m_pointSize=5;
    bool m_pointOutline=true;
    QColor m_pointColor=Qt::blue;
    QVector<ScatterPlotPoint> m_data;
    Range<double> m_xRange;
    Range<double> m_yRange;
    QMap< QString, QColor > m_keyColors;
    QSet< QString > m_displayedKeys;
};

#endif // SCATTERPLOTVIEW_H
