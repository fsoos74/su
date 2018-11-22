#ifndef VOLUMEVIEWER2D_H
#define VOLUMEVIEWER2D_H

#include <QMainWindow>
#include <avoproject.h>
#include <reversespinbox.h>
#include <QComboBox>
#include <histogramrangeselectiondialog.h>
#include "volumeview.h"
#include <colorbarwidget.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <playerdialog.h>
#include <dynamicmousemodeselector.h>
#include <volumepicker.h>


namespace Ui {
class VolumeViewer2D;
}

class VolumeViewer2D : public QMainWindow
{
    Q_OBJECT

public:
    explicit VolumeViewer2D(QWidget *parent = 0);
    ~VolumeViewer2D();

    void setProject(AVOProject*);

public slots:
    void addVolume(QString);

protected:
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);
private slots:

    void setInlineSliceX(QPointF);
    void setInlineSliceY(QPointF);
    void setCrosslineSliceX(QPointF);
    void setCrosslineSliceY(QPointF);
    void setZSlice(QPointF);
    void setFlattenHorizon(QString);

    void onMouseOver(QPointF);

    void onVolumesChanged();
    void onSliceChanged(VolumeView::SliceDef);
    void updateSliceConnections();

    void cbSlice_currentIndexChanged(QString);
    void sbSlice_valueChanged(int arg1);

    void on_actionDisplay_Range_triggered();
    void on_actionColorbar_triggered();
    void on_actionSetup_Volumes_triggered();
    void on_actionSetup_Horizons_triggered();
    void on_actionSet_Horizon_Color_triggered();
    void on_actionSetup_Wells_triggered();
    void on_actionSet_Well_Color_triggered();
    void on_actionSet_Marker_Color_triggered();
    void on_actionSet_Last_Viewed_Color_triggered();
    void on_actionVolume_Opacity_triggered();
    void on_action_Configure_Volumes_triggered();

    void on_action_Player_triggered();
    void on_actionSetup_Tops_triggered();
    void on_actionSetup_Tables_triggered();
    void on_actionSet_Table_Color_triggered();

    void on_action_New_Picks_triggered();
    void on_action_Load_Picks_triggered();
    void on_action_Save_Picks_triggered();
    void on_actionSave_As_Picks_triggered();

private:
    bool canDiscardPicks();
    void setupMouseModes();
    void setupSliceToolBar();
    void setupFlattenToolBar();
    void setupWellToolBar();
    void setupPickingToolBar();
    void setupEnhanceToolBar();
    void updateFlattenHorizons();
    void populateWindowMenu();
    //void orientate();

    Ui::VolumeViewer2D *ui;
    DynamicMouseModeSelector* m_mousemodeSelector=nullptr;
    QVBoxLayout* m_colorbarsLayout;
    //QComboBox* m_cbPickMode;
    QComboBox* m_cbSlice;
    ReverseSpinBox* m_sbSlice;

    //QSpinBox* sbWellViewDist;
    //QSpinBox* sbSharpenPercent;
    //QSpinBox* sbSharpenKernelSize;
    QComboBox* m_cbHorizon;

    QToolBar* m_sliceToolBar;
    QToolBar* m_flattenToolBar;
    QToolBar* m_wellToolBar;
    QToolBar* m_enhanceToolBar;
    QToolBar* m_pickToolBar;
    HistogramRangeSelectionDialog* displayRangeDialog=nullptr;
    QMap<QString, Histogram> m_volumeHistograms;
    //QMap<QString, std::pair<double,double>> m_volumeRanges;
    QMap<QString, ColorBarWidget*> m_colorbars;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    AVOProject* m_project=nullptr;
    PlayerDialog* m_player=nullptr;

    VolumePicker* m_picker=nullptr;
    QString m_picksName;

    bool m_noupdate=false;
};

#endif // VOLUMEEDITOR_H
