#ifndef CROSSPLOTVIEWER_H
#define CROSSPLOTVIEWER_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QPolygonF>
//#include <projectdispatcher.h>
#include <QPointF>
#include <baseviewer.h>
#include <selectionpoint.h>
#include <memory>
#include <avoproject.h>
#include <colortable.h>
#include <QDockWidget>
#include<colorbarwidget.h>
//#include<displayrangedialog.h>
#include <histogramrangeselectiondialog.h>
#include "crossplotviewerdisplayoptionsdialog.h"

namespace Ui {
class CrossplotViewer;
}


#include<memory>
#include<functional>
#include<grid2d.h>
#include<range.h>
#include <crossplot.h>

#include <mousemodeselector.h>


class CrossplotViewer : public BaseViewer
{
    Q_OBJECT

public:

    explicit CrossplotViewer(QWidget *parent = 0);
    ~CrossplotViewer();

    bool isFlattenTrend();
    bool isDisplayTrendLine();

    bool isFixedColor()const{
        return m_fixedColor;
    }

    bool isDetailedPointInformation();

    QColor pointColor()const{
        return m_pointColor;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    QColor trendlineColor()const{
        return m_trendlineColor;
    }

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

public slots:

    void setData( crossplot::Data);
    void setDetailedPointInformation(bool);
    void setTrend( QPointF ); // p(intercept, gradient)
    void setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation );
    void setRegion( Grid2DBounds, Range<float>);
    void setFlattenTrend(bool);
    void setDisplayTrendLine(bool);
    void setDatapointSize( int );
    void setFixedColor(bool);
    void setPointColor(QColor);
    void setColorMapping( const std::pair<double,double>& m);
    void setColors( const QVector<QRgb>&);
    void setTrendlineColor(QColor);

signals:

    void dataChanged();
    void fixedColorChanged(bool);
    void pointColorChanged(QColor);
    void colorTableChanged( ColorTable*);
    void trendlineColorChanged(QColor);

protected:

    void closeEvent(QCloseEvent*);

private slots:

    void onMouseOver(QPointF);

    void sceneSelectionChanged();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionZoom_Fit_Window_triggered();

    void on_actionDisplay_Options_triggered();

    void updateScene();


    void on_actionCompute_Trend_From_Loaded_Data_triggered();
    void on_actionCompute_Trend_From_Displayed_Data_triggered();
    void on_actionCompute_Trend_From_Selected_Data_triggered();
    void on_action_Pick_Trend_triggered();
    void on_action_Flatten_Trend_toggled(bool arg1);
    void on_actionSelect_By_Inline_Crossline_Ranges_triggered();

    void on_actionAttribute_Colortable_triggered();

    void on_actionAttribute_Range_triggered();

    void on_actionSet_Angle_triggered();

    void on_action_HistogramXAxis_triggered();
    void on_action_HistogramYAxis_triggered();
    void on_action_HistogramAttribute_triggered();

private:

    QVector<double> collectHistogramData( std::function<double(const crossplot::DataPoint&)> );

    void scanBounds();
    void scanAttribute();

    void createDockWidgets();
    void populateWindowMenu();
    void setupMouseModes();


    void saveSettings();
    void loadSettings();

    const qreal ZOOM_IN_FACTOR=1.1;
    const qreal ZOOM_OUT_FACTOR=0.9;
    const qreal X_PADDING_FACTOR=0.1;
    const qreal Y_PADDING_FACTOR=0.1;

    Ui::CrossplotViewer *ui;

    QGraphicsScene* m_scene=nullptr;

    CrossplotViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
    HistogramRangeSelectionDialog* displayRangeDialog=nullptr;
    ColorBarWidget* m_attributeColorBarWidget=nullptr;
    QDockWidget* m_attributeColorBarDock=nullptr;

    crossplot::Data m_data;

    Grid2DBounds m_geometryBounds;
    Range<float> m_timeRange;

    Range<float> m_attributeRange;

    bool        m_fixedColor=true;      // points drawn with point color or colortable based on attribute
    QColor      m_pointColor=Qt::blue;  // datapoints are drawn with this color if fixed
    ColorTable* m_colorTable;           // holds colors and display range
    int m_datapointSize=11;
    qreal m_zoomFactor=1;

    QColor m_trendlineColor=Qt::red;

    QPointF m_trend;
};

#endif // CROSSPLOTVIEWER_H
