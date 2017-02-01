#ifndef VOLUMEVIEWER_H
#define VOLUMEVIEWER_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QDockWidget>
#include <memory>

#include<baseviewer.h>
#include<avoproject.h>
#include<grid3d.h>
#include<colortable.h>
#include<selectionpoint.h>
#include <slicedef.h>
#include <navigation3dcontrols.h>
#include <colorbarwidget.h>
#include <volumenavigationdialog.h>
#include <displayrangedialog.h>
#include <editslicesdialog.h>
#include <edithorizonsdialog.h>
#include <horizonmodel.h>
#include <slicemodel.h>


namespace Ui {
class VolumeViewer;
}

class VolumeViewer : public BaseViewer
{
    Q_OBJECT

public:

    explicit VolumeViewer(QWidget *parent = 0);
    ~VolumeViewer();

    bool isColorMappingLocked(){
        return m_colorMappingLock;
    }

public slots:

    void setVolume( std::shared_ptr<Grid3D<float> >);
    void setProject( std::shared_ptr<AVOProject> );
    void setHighlightedPoints(QVector<SelectionPoint> rpoints);
    void clear();

    void setHighlightedPointColor(QColor);
    void setHighlightedPointSize(qreal);

    void setColorMappingLocked(bool);

signals:

    void sliceAdded(SliceDef);
    void sliceRemoved(SliceDef);
    void horizonsChanged( );

private slots:

    void refreshView();

    void on_action_Volume_Colortable_triggered();

    void on_actionVolume_Range_triggered();

    void on_action_Front_triggered();

    void on_action_Back_triggered();

    void on_action_Left_triggered();

    void on_action_Right_triggered();

    void on_action_Top_triggered();

    void on_action_Bottom_triggered();

    void on_actionSet_Point_Size_triggered();

    void on_actionHistogram_triggered();

    void on_actionNavigation_Dialog_triggered();

    void on_action_Background_Color_triggered();

    void on_actionEdit_Slices_triggered();


    void on_actionEdit_Horizons_triggered();

    void on_action_Open_Volume_triggered();

protected:

    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

private:

    void createDockWidgets();
    void populateWindowMenu();

    void directionIndicatorPlanesToView( Grid3DBounds bounds);

    void outlineToView( Grid3DBounds bounds );
    void sliceToView( const SliceDef& );
    void inlineSliceToView( int iline );
    void crosslineSliceToView( int xline );
    void timeSliceToView( int msec);
    void horizonToView(Grid2D<float>* hrz, int delayMSec=0);
    void horizonToView(Grid2D<float>* hrz, QColor, int delayMSec=0);
    void pointsToView( QVector<SelectionPoint> points, QColor color, qreal SIZE );

    void initialVolumeDisplay();
    void defaultPositionAndScale();

    Ui::VolumeViewer *ui;

    DisplayRangeDialog* displayRangeDialog=nullptr;
    EditSlicesDialog* editSlicesDialog = nullptr;
    EditHorizonsDialog* editHorizonsDialog=nullptr;
    ColorBarWidget* m_volumeColorBarWidget=nullptr;
    QDockWidget* m_volumeColorBarDock=nullptr;
    Navigation3DControls* m_navigationControls=nullptr;
    QDockWidget* m_navigationControlsDock=nullptr;

    VolumeNavigationDialog* m_navigationDialog=nullptr;

    std::shared_ptr<Grid3D<float> > m_volume;
    std::shared_ptr<AVOProject> m_project;
    QTransform ilxl_to_xy, xy_to_ilxl;
    ColorTable* m_colorTable;           // holds colors and display range

    HorizonModel* m_horizonModel;
    SliceModel* m_sliceModel;

    QVector<SelectionPoint> m_highlightedPoints;
    qreal m_highlightedPointSize=2;
    QColor m_highlightedPointColor=Qt::red;

    bool m_colorMappingLock=false;
};

#endif // VOLUMEVIEWER_H
