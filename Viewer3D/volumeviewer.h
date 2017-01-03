#ifndef VOLUMEVIEWER_H
#define VOLUMEVIEWER_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <memory>

#include<baseviewer.h>
#include<avoproject.h>
#include<grid3d.h>
#include<colortable.h>
#include<selectionpoint.h>
#include <slicedef.h>

namespace Ui {
class VolumeViewer;
}

class VolumeViewer : public BaseViewer
{
    Q_OBJECT

public:

    struct HorizonDef{
        QString                         name;
        std::shared_ptr<Grid2D<float>>  horizon;
        QColor                          color;
    };


    explicit VolumeViewer(QWidget *parent = 0);
    ~VolumeViewer();

public slots:

    void setVolume( std::shared_ptr<Grid3D<float> >);
    void setProject( std::shared_ptr<AVOProject> );
    void setHighlightedPoints(QVector<SelectionPoint> rpoints);
    void clear();

    void addSlice( SliceDef );
    void removeSlice( SliceDef );

    void addHorizon( HorizonDef );
    void removeHorizon( QString );

    void setHighlightedPointColor(QColor);
    void setHighlightedPointSize(qreal);

private slots:

    void refreshView();

    void on_action_Volume_Colortable_triggered();

    void on_actionVolume_Range_triggered();

    void on_action_Add_Slice_triggered();

    void on_action_List_Slices_triggered();

    void on_actionAdd_Horizon_triggered();

    void on_actionRemove_Horizon_triggered();

    void on_actionChange_Horizon_Color_triggered();

    void on_action_Front_triggered();

    void on_action_Back_triggered();

    void on_action_Left_triggered();

    void on_action_Right_triggered();

    void on_action_Top_triggered();

    void on_action_Bottom_triggered();

    void on_actionSet_Point_Size_triggered();

protected:

    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

private:

    void setupNavigationToolBar();
    void populateWindowMenu();

    void outlineToView( Grid3DBounds bounds );
    void sliceToView( const SliceDef& );
    void inlineSliceToView( int iline );
    void crosslineSliceToView( int xline );
    void sampleSliceToView( int sample );
    void horizonToView(Grid2D<float>* hrz, QColor);
    void pointsToView( QVector<SelectionPoint> points, QColor color, qreal SIZE );

    void initialVolumeDisplay();
    void defaultPositionAndScale();

    Ui::VolumeViewer *ui;

    std::shared_ptr<Grid3D<float> > m_volume;
    std::shared_ptr<AVOProject> m_project;
    ColorTable* m_colorTable;           // holds colors and display range
    QVector<SliceDef> m_slices;
    QMap<QString, HorizonDef> m_horizons;
    QVector<SelectionPoint> m_highlightedPoints;
    qreal m_highlightedPointSize=2;
    QColor m_highlightedPointColor=Qt::red;
};

#endif // VOLUMEVIEWER_H
