#ifndef VOLUMEVIEWER_H
#define VOLUMEVIEWER_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QDockWidget>
#include <memory>

#include<baseviewer.h>
#include<avoproject.h>
#include<volume.h>
#include<wellpath.h>
#include<colortable.h>
#include<selectionpoint.h>
#include <slicedef.h>
#include <navigation3dcontrols.h>
#include <colorbarwidget.h>
#include <volumenavigationdialog.h>
//#include <displayrangedialog.h>
#include<histogramrangeselectiondialog.h>
#include <editslicesdialog.h>
#include <edithorizonsdialog.h>
#include <horizonmodel.h>
#include <slicemodel.h>
#include <volumedimensions.h>

namespace Ui {
class VolumeViewer;
}

class VolumeViewer : public BaseViewer
{
    Q_OBJECT

public:

    explicit VolumeViewer(QWidget *parent = 0);
    ~VolumeViewer();

    QStringList volumeList(){
        return m_volumes.keys();
    }

public slots:

    void setProject( AVOProject* );

    void clear();

    void setDimensions(VolumeDimensions);

    void addVolume( QString, std::shared_ptr<Volume >);
    void removeVolume( QString );

    void setHighlightedPoints(QVector<SelectionPoint> rpoints);
    void setHighlightedPointColor(QColor);
    void setHighlightedPointSize(qreal);

    void setCompassColor( QColor );
    void setCompassSize( qreal );

    void setLabelColor( QColor );
    void setLabelSize( qreal );

    void setOutlineColor( QColor );

signals:

    void dimensionsChanged( VolumeDimensions );
    void volumeListChanged( QStringList );

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
    void on_actionReset_Highlighted_Points_triggered();
    void on_actionLimit_Highlighted_Points_triggered();
    void on_actionSet_Point_Color_triggered();
    void on_actionSet_Compass_Size_triggered();
    void on_actionSet_Compass_Color_triggered();
    void on_actionSet_Outline_Color_triggered();
    void on_actionSet_Label_size_triggered();
    void on_actionSet_Label_Color_triggered();
    void on_actionSet_Dimensions_triggered();
    void on_action_Close_Volume_triggered();
    void on_actionSetup_Wells_triggered();

protected:

    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

private:

    void createDockWidgets();
    void populateWindowMenu();

    void directionIndicatorPlanesToView( Grid3DBounds bounds);
    void outlineToView( VolumeDimensions, QColor );
    void sliceToView( const SliceDef& );
    void inlineSliceToView( const SliceDef& );
    void crosslineSliceToView( const SliceDef& );
    void timeSliceToView(const SliceDef&);
    void horizonToView(const HorizonDef&);
    void pointsToView( QVector<SelectionPoint> points, QColor color, qreal SIZE );
    void wellPathToView( const WellPath& wp, QColor color, qreal SIZE);
    void compassToView( QVector3D pos, qreal SIZE, QColor color);
    void textToView( QVector3D pos1, QVector3D pos2, QString text,  Qt::Alignment valign=Qt::AlignVCenter );
    void initialVolumeDisplay();
    void defaultPositionAndScale();
    QString selectVolume();

    Ui::VolumeViewer *ui;

    QMap<QString, HistogramRangeSelectionDialog*> m_displayRangeDialogs;
    QMap<QString, ColorBarWidget*> m_volumeColorBarWidgets;
    QMap<QString, QDockWidget*> m_volumeColorBarDocks;

    //DisplayRangeDialog* displayRangeDialog=nullptr;
    EditSlicesDialog* editSlicesDialog = nullptr;
    EditHorizonsDialog* editHorizonsDialog=nullptr;

    Navigation3DControls* m_navigationControls=nullptr;
    QDockWidget* m_navigationControlsDock=nullptr;

    VolumeNavigationDialog* m_navigationDialog=nullptr;

    AVOProject* m_project;
    QTransform ilxl_to_xy, xy_to_ilxl;

    VolumeDimensions m_dimensions;

    QMap<QString, std::shared_ptr<Volume > > m_volumes;
    QMap<QString, ColorTable* > m_colorTables;
    QMap<QString, std::shared_ptr<WellPath> > m_wellPaths;
    HorizonModel* m_horizonModel;
    SliceModel* m_sliceModel;

    QVector<SelectionPoint> m_highlightedPoints;
    int m_maxHighlightedPoints=1000;
    qreal m_highlightedPointSize=50;                    // meters
    QColor m_highlightedPointColor=Qt::red;

    QColor m_compassColor=Qt::black;
    qreal m_compassSize=1000;

    QColor m_labelColor=Qt::black;
    qreal m_labelSize=12;

    QColor m_outlineColor=Qt::white;
};

#endif // VOLUMEVIEWER_H
