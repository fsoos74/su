#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QScrollArea>
#include<QLabel>
#include<QSettings>
#include<QDir>

#include<memory>
#include<grid2d.h>
#include<gridview.h>
#include<colortable.h>
#include<colorbarwidget.h>
#include<displayrangedialog.h>
#include<isolinedialog.h>
#include "griddisplayoptionsdialog.h"
#include "aspectratiodialog.h"
#include "scaledialog.h"
#include "orientationdialog.h"
#include "displayrangedialog.h"
#include <histogramdialog.h>
#include<colorbarconfigurationdialog.h>
#include <projectgeometry.h>

#include<baseviewer.h>
#include<selectionpoint.h>
#include<projectgeometry.h> // il/xl -> x/y
#include<avoproject.h>


namespace Ui {
class GridViewer;
}

class GridViewer : public BaseViewer
{
    Q_OBJECT

public:

    enum TimeSource{ TIME_NONE, TIME_GRID, TIME_FIXED };

    explicit GridViewer(QWidget *parent = 0);
    ~GridViewer();

    GridView* gridView()const{
        return m_gridView;
    }

    bool orientate(const ProjectGeometry&);

    TimeSource timeSource(){
        return m_timeSource;
    }

    int fixedTime()const{           // millis
        return m_fixedTime;
    }

    QToolBar* navigationToolBar();

public slots:
    void setGrid( std::shared_ptr<Grid2D<float> >);
    void setProject( std::shared_ptr<AVOProject> );
    void setTimeSource(TimeSource);
    void setFixedTime(int);

signals:
    void timeSourceChanged(TimeSource);
    void fixedTimeChanged(int);
    void requestSlice(int msecs);

protected:

    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

    void closeEvent(QCloseEvent*);
    void leaveEvent(QEvent*);

private slots:

    void onViewPointSelected( SelectionPoint );     // need this to forward point from view to dispatcher

    void onViewPolylineSelected( QVector<QPoint>);  // see above

    void onGridViewMouseOver( int, int );


    void on_zoomInAct_triggered();

    void on_zoomOutAct_triggered();

    void on_zoomNormalAct_triggered();

    void on_displayRangeAct_triggered();

    void on_EditColorTableAct_triggered();

    //void on_setBackgroundColorAct_triggered();

    void on_actionSetup_Contours_triggered();

    void on_actionAxxis_Orientation_triggered();

    void on_action_Grid_Display_triggered();

    void on_actionDisplay_Histogram_triggered();

    void on_actionConfigure_Colorbar_triggered();

    void on_actionAspect_Ratio_triggered();

    void on_actionConfigure_Scales_triggered();

private:

    void populateWindowMenu();

    double pointTime( int iline, int xline );

    QString createStatusMessage( SelectionPoint);

    void setDefaultColorTable();

    void saveSettings();
    void loadSettings();

    void updateIntersectionTimes();

    Ui::GridViewer *ui;
    GridView* m_gridView;
    ColorBarWidget* colorBar;

    std::shared_ptr<Grid2D<float> > m_grid;
    std::shared_ptr<AVOProject> m_project;

    TimeSource m_timeSource=TimeSource::TIME_NONE;
    int m_fixedTime=0;

    DisplayRangeDialog* displayRangeDialog=nullptr;
    AspectRatioDialog* aspectRatioDialog=nullptr;
    ScaleDialog* scaleDialog=nullptr;
    IsoLineDialog* isoLineDialog=nullptr;
    GridDisplayOptionsDialog* gridDisplayOptionsDialog=nullptr;
    ColorBarConfigurationDialog* colorBarConfigurationDialog=nullptr;
};


#endif // MAINWINDOW_H
