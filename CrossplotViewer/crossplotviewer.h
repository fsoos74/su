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
#include <volumedimensions.h>
#include <crossplot.h>
#include<crossplotview.h>
#include <mousemodeselector.h>


class CrossplotViewer : public BaseViewer
{
    Q_OBJECT

public:

    explicit CrossplotViewer(QWidget *parent = 0);
    ~CrossplotViewer();

    bool isDetailedPointInformation();

    bool isInlinesSelectable()const{
        return m_inlinesSelectable;
    }

    bool isCrosslinesSelectable()const{
        return m_crosslinesSelectable;
    }

    bool isMsecsSelectable()const{
        return m_msecsSelectable;
    }

    CrossplotView* view();

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

public slots:

    void setData( crossplot::Data);
    void setDetailedPointInformation(bool);
    void setInlinesSelectable(bool);
    void setCrosslinesSelectable(bool);
    void setMSecsSelectable(bool);
    void setAxisLabels( const QString& xAxisAnnotation, const QString& yAxisAnnotation );

signals:

    void dataChanged();

protected:

    void closeEvent(QCloseEvent*);

private slots:

    void onTrendLineSelected(QLineF);
    void onMouseOver(QPointF);
    void sceneSelectionChanged();
    void on_actionDisplay_Options_triggered();


    void on_actionCompute_Trend_From_Loaded_Data_triggered();
    void on_actionCompute_Trend_From_Displayed_Data_triggered();
    void on_actionCompute_Trend_From_Selected_Data_triggered();
    void on_action_Pick_Trend_triggered();

    void on_actionSelect_By_Inline_Crossline_Ranges_triggered();

    void on_actionAttribute_Colortable_triggered();
    void on_actionAttribute_Range_triggered();
    void on_actionSet_Angle_triggered();

    void on_action_HistogramXAxis_triggered();
    void on_action_HistogramYAxis_triggered();
    void on_action_HistogramAttribute_triggered();

private:

    QVector<double> collectHistogramData( std::function<double(const crossplot::DataPoint&)> );

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

    bool m_inlinesSelectable=true;
    bool m_crosslinesSelectable=true;
    bool m_msecsSelectable=true;

    CrossplotViewerDisplayOptionsDialog* displayOptionsDialog=nullptr;
    HistogramRangeSelectionDialog* displayRangeDialog=nullptr;
    ColorBarWidget* m_attributeColorBarWidget=nullptr;
    QDockWidget* m_attributeColorBarDock=nullptr;
};

#endif // CROSSPLOTVIEWER_H
