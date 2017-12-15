#ifndef VOLUMEVIEWER2D_H
#define VOLUMEVIEWER2D_H

#include <QMainWindow>
#include <avoproject.h>
#include <QSpinBox>
#include <QComboBox>
#include <histogramrangeselectiondialog.h>
#include "volumeview.h"
#include <colorbarwidget.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <playerdialog.h>

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

private:
    void setupToolBarControls();
    void updateFlattenHorizons();
    //void orientate();

    Ui::VolumeViewer2D *ui;
    QVBoxLayout* m_colorbarsLayout;

    QComboBox* cbSlice;
    QSpinBox* sbSlice;
    //QPushButton* pbSlice;
    QSpinBox* sbWellViewDist;
    QComboBox* cbHorizon;
    HistogramRangeSelectionDialog* displayRangeDialog=nullptr;
    QMap<QString, Histogram> m_volumeHistograms;
    //QMap<QString, std::pair<double,double>> m_volumeRanges;
    QMap<QString, ColorBarWidget*> m_colorbars;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    AVOProject* m_project=nullptr;
    PlayerDialog* m_player=nullptr;

    bool m_noupdate=false;
};

#endif // VOLUMEEDITOR_H
