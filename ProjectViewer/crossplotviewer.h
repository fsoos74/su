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

#include "crossplotviewerdisplayoptionsdialog.h"

namespace Ui {
class CrossplotViewer;
}


#include<memory>
#include<grid2d.h>
#include<range.h>
#include <crossplot.h>

class CrossplotViewer : public BaseViewer
{
    Q_OBJECT

public:

    explicit CrossplotViewer(QWidget *parent = 0);
    ~CrossplotViewer();

    bool isFlattenTrend();
    bool isDisplayTrendLine();

    ColorTable* colorTable()const{
        return m_colorTable;
    }

protected:
    void receivePoint( SelectionPoint );
    void receivePoints( QVector<SelectionPoint>, int code);

public slots:

    void setData( crossplot::Data);
    void setTrend( QPointF ); // p(intercept, gradient)
    void setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation );
    void setRegion( Grid2DBounds, Range<float>);
    void setFlattenTrend(bool);
    void setDisplayTrendLine(bool);
    void setDatapointSize( int );
    void setColorMapping( const std::pair<double,double>& m);
    void setColors( const QVector<QRgb>&);

signals:

    void dataChanged();
    void colorTableChanged( ColorTable*);

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

private:

    void scanBounds();

    void saveSettings();
    void loadSettings();

    const qreal ZOOM_IN_FACTOR=1.1;
    const qreal ZOOM_OUT_FACTOR=0.9;
    const qreal X_PADDING_FACTOR=0.1;
    const qreal Y_PADDING_FACTOR=0.1;

    Ui::CrossplotViewer *ui;

    QGraphicsScene* m_scene=nullptr;

    std::shared_ptr<AVOProject> m_project;

    //std::shared_ptr< Grid2D<QPointF> > m_data;
    crossplot::Data m_data;
    Grid2DBounds m_geometryBounds;
    Range<float> m_timeRange;

    ColorTable* m_colorTable;           // holds colors and display range

    int m_datapointSize=11;
    qreal m_zoomFactor=1;
    QPointF m_trend;

    CrossplotViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
};

#endif // CROSSPLOTVIEWER_H
