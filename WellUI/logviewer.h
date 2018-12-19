#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <baseviewer.h>

#include<avoproject.h>
#include <log.h>
#include <wellpath.h>
#include <wellinfo.h>
#include <tracklabel.h>
#include <trackview.h>
#include <hscaleview.h>
#include <vscaleview.h>
#include <axis.h>
#include <grid2d.h>
#include<QFont>
#include<QLabel>
#include<QComboBox>
#include<QSpinBox>
#include<QScrollBar>
#include <QPushButton>
#include<memory>
#include <dynamicmousemodeselector.h>

namespace Ui {
class LogViewer;
}

class LogViewer : public BaseViewer
{
    Q_OBJECT

public:

    enum TrackLabelMode{ UWI=0, WELL_NAME};
    enum FlattenMode{ FLATTEN_NONE=0, FLATTEN_HORIZON, FLATTEN_TOP};
    enum PickMode{ PICK_TOPS, PICK_LOG};

    explicit LogViewer(QWidget *parent = 0);
    ~LogViewer();

    int penSize();

    ZMode zmode()const{
        return m_zmode;
    }

    int trackLabelMode()const{
        return m_trackLabelMode;
    }

public slots:
    void setProject( AVOProject*);
    void addLog(QString uwi, QString name);
    void removeLog(int);
    void removeLog(QString);
    void setPenSize(QString);
    void setPenSize(int);
    void setZMode(ZMode);
    void setZMode(QString);
    void setFlattenMode(FlattenMode);
    void setFlattenMode(QString);
    void setFlattenSource(QString);
    void setFilterLen(int);
    void setTrackLabelMode(int);
    void setPickMode(PickMode);
    void setPickMode(QString);

signals:
    void filterLenChanged(int);
    void zmodeChanged(ZMode);
    void zmodeChanged(QString);

protected:
    void receivePoint( SelectionPoint, int code ) override;
    void receivePoints( QVector<SelectionPoint>, int code)override;

    void resizeEvent(QResizeEvent *)override;

private slots:

    void on_action_Add_Log_triggered();
    void on_actionAdd_Logs_by_Log_triggered();
    void on_action_Remove_Log_triggered();
    void on_actionConfigure_X_Axis_2_triggered();
    void on_actionConfigure_Z_Axis_triggered();
    void on_actionConfigure_Track_Labels_triggered();
    void onXCursorChanged(qreal);
    void onZCursorChanged(qreal);
    void onTrackLabelMoved(QPoint);
    void onAddTrackLog();
    void onRemoveTrackLog();
    void onTrackColors();
    void onShrinkTrack();
    void onGrowTrack();
    void onCloseTrack();
    void onTrackPointSelected(QPointF);
    void on_actionSort_By_Log_triggered();
    void on_actionSort_By_Well_triggered();
    void on_actionSetup_Horizons_triggered();
    void on_actionSetup_Tops_triggered();
    void runTrackViewContextMenu(QPoint);

private:

    int selectLog(TrackView*,QString title="Select Log");
    void convertPicksToNewLog(TrackView*);
    void addPicksToLog(TrackView*);
    void saveLog(TrackView*);

    void setupMouseModes();
    QString trackLabelText(const WellInfo&, const Log&);
    qreal intersect(const Grid2D<float>& h, const WellPath& wp);
    void updateTrackLabels();
    void updateTrackHorizons();
    void updateTrackTops();
    void updateTrackFlatten();
    void updateZAxis();
    void addLog( WellInfo, std::shared_ptr<WellPath>, std::shared_ptr<Log>, std::shared_ptr<WellMarkers> );
    void layoutLogs();
    void setupTrackToolbar();
    void setupZModeToolbar();
    void setupFlattenToolbar();
    void setupFilterToolbar();
    void setupPickingToolbar();
    void updateTrackPickMode(TrackView* tv);

    Ui::LogViewer *ui;
    DynamicMouseModeSelector* m_mousemodeSelector=nullptr;
    QWidget* m_tracksAreaWidget;
    QComboBox* m_cbZMode;
    QComboBox* m_cbPenSize;
    QComboBox* m_cbFlattenMode;
    QComboBox* m_cbFlattenSource;
    QSpinBox* m_sbFilterLen;
    QComboBox* m_cbPickMode;

    Axis* m_zAxis=nullptr;
    QLayout* m_tracksLayout=nullptr;
    AVOProject* m_project=nullptr;
    ZMode m_zmode=ZMode::MD;
    FlattenMode m_flattenMode=FLATTEN_NONE;
    QString m_flattenSource;
    int m_filterLen=0;
    int m_trackLabelMode=TrackLabelMode::UWI;
    PickMode m_PickMode=PickMode::PICK_LOG;

    QMap<QString, std::shared_ptr<Grid2D<float>>> m_horizons;
    QVector<std::shared_ptr<WellMarkers>> m_markers;
    QStringList m_selectedTops;
    QVector<WellInfo> m_wellInfos;
    QVector<TrackView*> m_trackViews;
    QVector<TrackLabel*> m_trackLabels;
    QVector<QLabel*> m_trackValueLabels;
    QVector<HScaleView*> m_trackScaleViews;
    QVector<QPushButton*> m_shrinkButtons;
    QVector<QPushButton*> m_growButtons;
    QVector<QPushButton*> m_closeButtons;
    QVector<QPushButton*> m_addButtons;
    QVector<QPushButton*> m_removeButtons;
    QVector<QPushButton*> m_colorButtons;
    VScaleView* m_vscaleView=nullptr;
    QScrollBar* m_verticalScrollBar=nullptr;
    QFont m_labelFont = QFont("Helvetica [Cronyx]", 10, QFont::Bold);
    QFont m_trackValueFont = QFont("Helvetica [Cronyx]", 10 );
};

#endif // LOGVIEWER_H
