#ifndef ScatterplotVIEWER_H
#define ScatterplotVIEWER_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QPointF>
#include <baseviewer.h>
#include <memory>
#include <QDockWidget>
#include <scatterplotview.h>
#include <scatterplotviewerdisplayoptionsdialog.h>
#include<functional>

namespace Ui {
class ScatterplotViewer;
}



#include <mousemodeselector.h>


class ScatterplotViewer : public QMainWindow
{
    Q_OBJECT

public:

    explicit ScatterplotViewer(QWidget *parent = 0);
    ~ScatterplotViewer();

    ScatterplotView* view();

public slots:

    void setData( QVector<ScatterPlotPoint> );
    void setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation );

protected:

    void closeEvent(QCloseEvent*);

private slots:

    void sceneSelectionChanged();
    void onMouseOver(QPointF);
    void on_action_HistogramXAxis_triggered();
    void on_action_HistogramYAxis_triggered();
    void on_actionDisplay_Options_triggered();
    void on_actionSelect_Items_triggered();
    void updateLegend();

private:

    QVector<double> collectHistogramData( std::function<double(const QPointF&)> f);
    void setupMouseModes();
    void createDockWidgets();
    void populateWindowMenu();

    const qreal ZOOM_IN_FACTOR=1.1;
    const qreal ZOOM_OUT_FACTOR=0.9;
    const qreal X_PADDING_FACTOR=0.1;
    const qreal Y_PADDING_FACTOR=0.1;

    Ui::ScatterplotViewer *ui;
    QGraphicsView* m_legendWidget=nullptr;
    QDockWidget* m_legendDock=nullptr;
    ScatterplotViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
};

#endif // ScatterplotVIEWER_H
