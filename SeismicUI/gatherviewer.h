#ifndef GATHERVIEWER_H
#define GATHERVIEWER_H

#include <QMainWindow>

#include<gather.h>
#include<gatherview.h>
#include<gatherscaler.h>
#include<segyreader.h>
#include <segy_header_def.h>
#include<memory>
#include<string>
#include<vector>
#include<headerdialog.h>
#include"tracescalingdialog.h"
#include"tracedisplayoptionsdialog.h"
#include "volumedisplayoptionsdialog.h"
#include "pointdisplayoptionsdialog.h"
#include "gathersortkey.h"

#include <selectionpointbuffer.h>

#include <baseviewer.h>
#include <QDockWidget>
#include<colorbarwidget.h>

using seismic::Gather;

namespace Ui {
class GatherViewer;
}

class AVOProject;   // need this to load horizons

class GatherViewer : public BaseViewer
{
    Q_OBJECT

public:


    GatherViewer( QWidget *parent = 0);

    ~GatherViewer();

    std::shared_ptr<seismic::Gather> gather()const{
        return m_gather;
    }

    const std::vector<seismic::SEGYHeaderWordDef>& traceHeaderDef()const{
        return m_traceHeaderDef;
    }

    QToolBar* navigationToolBar()const;

    GatherView* view()const{
        return gatherView;
    }

    const std::vector< std::pair< std::string, QString > >& traceAnnotations()const{
        return m_traceAnnotations;
    }

    bool isShareCurrentPoint()const;

protected:

    void receivePoint(SelectionPoint, int code);
    void receivePoints(QVector<SelectionPoint>, int code);

public slots:
    void setProject( std::shared_ptr<AVOProject>);
    void setGather( std::shared_ptr<seismic::Gather>);
    void setTraceHeaderDef(const std::vector<seismic::SEGYHeaderWordDef>&);
    void zoomFitWindow();
    void setTraceAnnotations( const std::vector< std::pair< std::string, QString > >& );  
    void setShareCurrentPoint(bool);

signals:
    void projectChanged();
    void gatherChanged();
    void traceAnnotationsChanged();
    void requestPoint(int,int);
    void requestPoints(QVector<QPoint>);
    void requestPerpendicularLine(int,int);

protected:

    void closeEvent(QCloseEvent*);
    void leaveEvent(QEvent*);

private slots:

    void showTraceHeader( size_t );        // 0-based
    void navigateToTraceHeader(size_t);    // 1-based
    void onTraceHeaderDialogFinished();

    void onTraceSelected(size_t);
    void onViewPointSelected( SelectionPoint );

    void onMouseOver(int, qreal);

    void onTopRulerClicked( int  );

    void on_zoomInAct_triggered();

    void on_zoomOutAct_triggered();

    void on_zoomFitWindowAct_triggered();

    void on_openGridAct_triggered();

    void on_closeGridAct_triggered();

    void on_actionOpenVolume_triggered();

    void on_actionCloseVolume_triggered();

    void on_actionVolume_Options_triggered();

    void on_action_Trace_Options_triggered();

    void on_actionTrace_Scaling_triggered();

    void on_action_Point_Display_Options_triggered();

    void on_actionShare_Current_Position_toggled(bool arg1);

    void on_action_Receive_CDPs_toggled(bool arg1);

    void on_action_Dispatch_CDPs_toggled(bool arg1);

private:

    void saveSettings();
    void loadSettings();

    void createDockWidgets();

    QString createStatusMessage( SelectionPoint);

    QVector<int> computeIntersections();
    void updateIntersections();


    Ui::GatherViewer *ui;

    GatherView* gatherView;

    std::shared_ptr<Gather> m_gather;


    std::shared_ptr<AVOProject> m_project;

    std::vector<seismic::SEGYHeaderWordDef> m_traceHeaderDef;

    std::vector< std::pair< std::string, QString> > m_traceAnnotations;

    HeaderDialog* m_TraceHeaderDialog=nullptr;

    TraceScalingDialog* m_traceScalingDialog=nullptr;
    TraceDisplayOptionsDialog* m_traceDisplayOptionsDialog=nullptr;
    VolumeDisplayOptionsDialog* m_volumeDisplayOptionsDialog=nullptr;
    PointDisplayOptionsDialog* m_pointDisplayOptionsDialog=nullptr;

    ColorBarWidget* m_densityColorBarWidget=nullptr;
    ColorBarWidget* m_attributeColorBarWidget=nullptr;
    QDockWidget* m_densityColorBarDock=nullptr;
    QDockWidget* m_attributeColorBarDock=nullptr;
};

#endif // GATHERVIEWER_H
