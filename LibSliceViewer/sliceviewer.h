#ifndef SLICEEVIEWER_H
#define SLICEVIEWER_H

#include <QMainWindow>
#include <avoproject.h>
#include <reversespinbox.h>
#include <QComboBox>
#include <QMdiSubWindow>
#include <histogramrangeselectiondialog.h>
#include "volumeview.h"
#include <colorbarwidget.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <playerdialog.h>
#include <dynamicmousemodeselector.h>
#include <volumepicker.h>


namespace sliceviewer {

namespace Ui {
class SliceViewer;
}

class VolumeItemsDialog;
class HorizonItemsDialog;
class WellItemsDialog;
class MarkerItemsDialog;
class TableItemsDialog;

class SliceViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit SliceViewer(QWidget *parent = 0);
    ~SliceViewer();

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

    void on_actionSetup_Volumes_triggered();
    void on_actionSetup_Horizons_triggered();
    void on_actionSetup_Wells_triggered();
    void on_actionSetup_Tops_triggered();
    void on_actionSetup_Tables_triggered();
    void on_actionDisplay_Options_triggered();
    void on_action_Player_triggered();

    void on_action_New_Picks_triggered();
    void on_action_Load_Picks_triggered();
    void on_action_Save_Picks_triggered();
    void on_actionSave_As_Picks_triggered();

    void on_mdiArea_customContextMenuRequested(const QPoint &pos);
    void colorBarContextMenuRequested(const QPoint &pos);

    void updatePickModeActions();
    void updatePickModePicker();

private:

    bool canDiscardPicks();
    void setupMouseModes();
    void setupSliceToolBar();
    void setupFlattenToolBar();
    void setupPickingToolBar();
    void updateFlattenHorizons();

    Ui::SliceViewer *ui;
    DynamicMouseModeSelector* m_mousemodeSelector=nullptr;
    QComboBox* m_cbSlice;
    ReverseSpinBox* m_sbSlice;

    QComboBox* m_cbHorizon;

    QToolBar* m_sliceToolBar;
    QToolBar* m_flattenToolBar;
    HistogramRangeSelectionDialog* displayRangeDialog=nullptr;
    QMap<QString, Histogram> m_volumeHistograms;
    QMap<QString, QMdiSubWindow*> m_mdiColorbars;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    AVOProject* m_project=nullptr;
    PlayerDialog* m_player=nullptr;

    VolumePicker* m_picker=nullptr;
    QString m_picksName;

    bool m_noupdate=false;

    VolumeItemsDialog* mVolumeItemsDialog=nullptr;
    HorizonItemsDialog* mHorizonItemsDialog=nullptr;
    WellItemsDialog* mWellItemsDialog=nullptr;
    MarkerItemsDialog* mMarkerItemsDialog=nullptr;
    TableItemsDialog* mTableItemsDialog=nullptr;
};


}

#endif // VOLUMEEDITOR_H
