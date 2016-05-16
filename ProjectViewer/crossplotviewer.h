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

#include "crossplotviewerdisplayoptionsdialog.h"

namespace Ui {
class CrossplotViewer;
}


#include<memory>
#include<grid2d.h>

class CrossplotViewer : public BaseViewer
{
    Q_OBJECT

public:

    struct DataPoint{
        DataPoint():x(0),y(0), iline(0),xline(0), time(0){}
        DataPoint( float ix, float iy, int iiline, int ixline, float(itime)):
            x(ix), y(iy), iline(iiline), xline(ixline), time(itime){}

        float x=0;
        float y=0;

        int iline=0;
        int xline=0;
        float time=0;
    };

    explicit CrossplotViewer(QWidget *parent = 0);
    ~CrossplotViewer();


protected:
    void receivePoint( SelectionPoint );
    void receivePoints( QVector<SelectionPoint>, int code);

public slots:

    void setData( QVector<DataPoint>);
    void setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation );
    void setGeometryBounds( Grid2DBounds);
    void setFlattenTrend(bool);
    void setDatapointSize( int );


signals:

    void dataChanged();

protected:

    void closeEvent(QCloseEvent*);

private slots:

    void onMouseOver(QPointF);

    void sceneSelectionChanged();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionZoom_Fit_Window_triggered();

    void on_actionSelect_Data_by_Geometry_triggered();

    void on_actionDisplay_Options_triggered();

private:

    void computeTrend();
    void updateScene();

    void saveSettings();
    void loadSettings();

    const qreal ZOOM_IN_FACTOR=1.1;
    const qreal ZOOM_OUT_FACTOR=0.9;
    const qreal X_PADDING_FACTOR=0.1;
    const qreal Y_PADDING_FACTOR=0.1;

    Ui::CrossplotViewer *ui;

    QGraphicsScene* m_scene=nullptr;
    //std::shared_ptr< Grid2D<QPointF> > m_data;
    QVector<DataPoint> m_data;
    Grid2DBounds m_geometryBounds;
    int m_datapointSize=11;
    qreal m_zoomFactor=1;
    bool m_flattenTrend=false;
    QPointF m_trend;

    CrossplotViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
};

#endif // CROSSPLOTVIEWER_H
