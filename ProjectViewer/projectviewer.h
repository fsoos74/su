#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include <QTableView>
#include <QMainWindow>
#include <QString>
#include <avoproject.h>
#include<memory>
#include<QVector>
#include<QStandardItemModel>
#include<QPoint>
#include<QLockFile>
#include<pointdispatcher.h>

#ifdef USE_KEYLOCK_LICENSE
#include "keylokclass.h"
#include "keylok.h"
#include "licenseinfo.h"
#endif

namespace Ui {
class ProjectViewer;
}

class ProjectProcess;


class QListView;

class ProjectViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectViewer(QWidget *parent = 0);
    ~ProjectViewer();

signals:

    void highlightCDPs( QVector<QPoint> InlinesAndCrosslines);

private slots:

    // File menu
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionImportSeismic_triggered();
    void on_actionImport_Attached_Datasets_triggered();
    void on_actionImportHorizon_triggered();
    void on_actionImportAttributeGrid_triggered();
    void on_actionImportOtherGrid_triggered();
    void on_actionImportVolume_triggered();
    void on_actionImportLogs_triggered();
    void on_action_Logs_Bulk_Mode_triggered();
    void on_actionWellpath_triggered();
    void on_actionWellpaths_Bulk_Mode_triggered();
    void on_actionWell_Markers_triggered();
    void on_actionExportHorizon_triggered();
    void on_actionExportAttributeGrid_triggered();
    void on_actionExportOtherGrid_triggered();
    void on_actionExportVolume_triggered();
    void on_actionExportSeismic_triggered();
    void on_actionExportLog_triggered();
    void on_action_ExportTable_triggered();
    void on_actionSaveProjectAs_triggered();
    void on_actionSave_triggered();
    void on_actionCloseProject_triggered();

    // Project menu
    void on_action_Geometry_triggered();
    void on_actionAxis_Orientation_triggered();
    void on_actionDisplay_Map_triggered();

    // Process seismic menu
    void on_actionCrop_Dataset_triggered();
    void on_actionCreateTimeslice_triggered();
    void on_actionHorizon_Amplitudes_triggered();
    void on_actionHorizon_Frequencies_triggered();
    void on_actionHorizon_Semblance_triggered();
    void on_actionVolume_Amplitudes_triggered();        // convert to volume
    void on_actionCompute_Intercept_Gradient_triggered();
    void on_actionCompute_Intercept_and_Gradient_Volumes_triggered();
    void on_action_Offset_Stack_triggered();

    // Process grid menu
    void on_actionCreate_New_Grid_triggered();
    void on_actionConvert_Grid_triggered();
    void on_actionCrop_Grid_triggered();
    void on_actionSmooth_Grid_triggered();
    void on_actionGrid_Math_triggered();
    void on_actionHorizon_Curvature_triggered();
    void on_actionFluid_Factor_Grid_triggered();    // not used, replaced by trend based
    void on_actionTrend_Based_Attribute_Grids_triggered();
    void on_actionSecondary_Attribute_Grids_triggered();
    void on_actionRun_Grid_User_Script_triggered();

    // Process volume menu
    void on_actionCreate_New_Volume_triggered();
    void on_action_Crop_Volume_triggered();
    void on_actionVolume_Math_triggered();
    void on_actionFlatten_Volume_triggered();
    void on_actionUnflatten_Volume_triggered();
    void on_actionExtract_Timeslice_triggered();
    void on_actionExtract_Dataset_triggered();      // convert to dataset
    void on_actionVolume_Dip_triggered();
    void on_actionVolume_Frequencies_triggered();
    void on_actionVolume_Semblance_triggered();
    void on_actionVolume_Variance_triggered();      // not used, integrated in statistics
    void on_actionVolume_Statistics_triggered();
    void on_actionVolume_Curvature_triggered();
    void on_actionInstantaneous_Attribute_Volumes_triggered();
    void on_actionFluid_Factor_Volume_triggered();  // not used, integrated in trend based
    void on_actionTrend_Based_Attribute_Volumes_triggered();
    void on_actionSecondary_Attribute_Volumes_triggered();
    void on_actionRun_Volume_Script_triggered();
    void on_actionPunch_Out_Volume_triggered();

    // Process well menu
    void on_action_Smooth_Log_triggered();
    void on_actionLog_Math_triggered();
    void on_actionLog_Integration_triggered();
    void on_actionTrain_NN_Log_triggered();
    void on_actionNN_Log_old_triggered();
    void on_actionNN_Log_Interpolation_triggered();
    void on_actionVShale_Computation_triggered();
    void on_actionRun_Log_Script_triggered();
    void on_actionBuild_Volume_triggered();
    void on_actionTops_from_Horizon_triggered();
    void on_actionEdit_Tops_triggered();

    // process table menu
    void on_actionConvert_Picks_To_Horizon_triggered();

    // Analyze menu
    void on_actionCrossplot_Grids_triggered();
    void on_actionCrossplot_Volumes_triggered();
    void on_actionCrossplot_Logs_triggered();
    void on_actionAmplitude_vs_Offset_Plot_triggered();
    void on_actionFrequency_Spectrum_Plot_triggered();
    void on_actionLog_TVD_Plot_triggered();
    void on_actionColor_Composite_Grids_triggered();
    void on_actionOpen_2DViewer_triggered();
    void on_actionOpen_Slice_Viewer_triggered();
    void on_actionOpen_3D_Viewer_triggered();
    void on_actionOpen_Log_Viewer_triggered();

    // Help menu
    void on_action_About_triggered();


    void openRecent();

    void updateDatasetsView();
    void updateDatasetsView(QString);
    void updateGridsView(GridType);
    void updateGridsView(GridType, QString);
    void updateVolumesView();
    void updateVolumesView(QString);
    void updateWellsView();
    void updateWellsView(QString);
    void updateTablesView();
    void updateTablesView(QString);
    void updateProjectViews();

    void runDatasetContextMenu(const QPoint& pos);
    void on_datasetsView_doubleClicked(const QModelIndex &index);
    void runHorizonContextMenu(const QPoint& pos);
    void runAttributeContextMenu(const QPoint& pos);
    void runOtherGridContextMenu(const QPoint& pos);
    void on_horizonsView_doubleClicked(const QModelIndex &index);
    void on_attributesView_doubleClicked(const QModelIndex &index);
    void on_gridsView_doubleClicked(const QModelIndex &index);
    void on_volumesView_doubleClicked(const QModelIndex &index);
    void runVolumeContextMenu(const QPoint& pos);
    void on_wellsView_doubleClicked(const QModelIndex &index);
    void runWellContextMenu(const QPoint& pos);
    void on_tablesView_doubleClicked(const QModelIndex &index);
    void runTableContextMenu(const QPoint& pos);

#ifdef USE_KEYLOCK_LICENSE
    void on_licenseTimer();
    void on_checkLicense();
#endif




    void on_actionFD_CDP_Acoustic_triggered();

    void on_actiontest_triggered();



    void on_actionStack_To_Gather_triggered();

protected:

    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent*);

private:

#ifdef USE_KEYLOCK_LICENSE
    license::LicenseInfo checkLicense();
#endif

    void setProjectFileName( const QString& fileName);
    bool openProject( const QString& fileName);
    bool saveProject( const QString& fileName);

    void connectDatasetsView(AVOProject*);
    void connectGridViews(AVOProject*);
    void connectVolumesView(AVOProject*);
    void connectWellsView(AVOProject*);
    void connectTablesView(AVOProject*);
    void connectViews(AVOProject*);
    QStandardItemModel* buildDatesetsModel();
    QStandardItemModel* buildDatasetModelItem(QStandardItemModel* model, int row, QString name);
    QStandardItemModel* buildGridsModel(GridType);
    QStandardItemModel* buildGridModelItem(GridType, QStandardItemModel* model, int row, QString name);
    QStandardItemModel* buildVolumesModel();
    QStandardItemModel* buildVolumeModelItem(QStandardItemModel* model, int row, QString name);
    QStandardItemModel* buildWellsModel();
    QStandardItemModel* buildWellModelItem(QStandardItemModel* model, int row, QString name);
    QStandardItemModel* buildTablesModel();
    QStandardItemModel* buildTableModelItem(QStandardItemModel* model, int row, QString name);

    void displayProcessParams( const QString&, const ProcessParams&);

    void displaySeismicDataset( const QString& name);
    void displaySeismicDatasetSlice( const QString& name);
    void displaySeismicDatasetIndex( const QString& name);
    void displaySeismicDatasetEBCDICHeader( const QString& name);
    void editSeismicDatasetProperties(const QString& name);
    void renameSeismicDataset(const QString&);
    void removeSeismicDataset(const QString&);
    void removeSeismicDatasets(const QStringList&);
    void selectAndExportSeismicDataset();
    void exportSeismicDataset(QString);

    void displayGrid( GridType t, const QString&);
    void displayGridHistogram( GridType t, const QString&);
    void renameGrid( GridType t, const QString&);
    void removeGrid( GridType t, const QString&);
    void removeGrids( GridType t, const QStringList&);
    void importGrid(GridType);
    void selectAndExportGrid(GridType);
    void exportGrid(GridType, QString);
    void runGridContextMenu(GridType, QTableView*, const QPoint&);

    void displayVolume3D( const QString& );
    void displayVolumeSlice( const QString&);
    void displayVolumeRelativeSlice( const QString&);
    void displayVolumeHistogram( const QString&);
    void displayVolumes(const QStringList&);
    void editVolumeProperties(const QString&);
    void renameVolume( const QString&);
    void removeVolume( const QString&);
    void removeVolumes( const QStringList&);
    void selectAndExportVolume();
    void exportVolume(QString);
    void exportVolumeSeisware(QString);

    void selectAndDisplayLog( const QStringList& );
    void selectAndDisplayLogSpectrum( const QString& );
    void editWell( const QString&);
    bool importWellPath( const QString&, const QString& initial=QString());
    bool importWellPathBulk( QVector<std::pair<QString, QString>> uwi_path);
    void editTops( const QString&);
    void removeTops(const QStringList&);
    void crossplotLogs( const QStringList& wells);
    void logProperties( const QString&);
    void editLog( const QString&);
    void removeLogs( const QStringList&);
    void selectAndExportLog();
    //void removeWell( const QString&);
    void removeWells( const QStringList&);

    void displayTable(const QString&);
    void renameTable(const QString&);
    void removeTables(const QStringList&);
    void selectAndExportTable();
    void exportTable(const QString&);

    void runProcess( ProjectProcess* process, QMap<QString, QString> params);

    void saveSettings();
    void loadSettings();

    void adjustForCurrentProject(const QString& filePath);
    void updateRecentProjectActionList();

    void updateMenu();

    Ui::ProjectViewer *ui;


    //std::shared_ptr<AVOProject> m_project;
    AVOProject* m_project=nullptr;
    QString m_projectFileName;
    std::shared_ptr<QLockFile> m_lockfile;

    std::shared_ptr<PointDispatcher> m_dispatcher;

    QStringList recentProjectFileList;
    QList<QAction*> recentProjectActionList;
    const int maxRecentProjectCount=5;

};

#endif // PROJECTVIEWER_H
