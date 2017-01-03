#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include <QMainWindow>
#include <QString>
#include <avoproject.h>
#include<memory>
#include<QVector>
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

    void openRecent();

    void on_actionNewProject_triggered();

    void on_actionOpenProject_triggered();

    void on_actionOpenGrid_triggered();

    void on_actionImportSeismic_triggered();

    void on_actionOpenSeismicDataset_triggered();

    void on_actionImportHorizon_triggered();

    void on_actionImportAttributeGrid_triggered();

    void on_actionImportOtherGrid_triggered();

    void on_actionExportHorizon_triggered();

    void on_actionExportAttributeGrid_triggered();

    void on_actionExportOtherGrid_triggered();

    void on_actionExportVolume_triggered();

    void on_actionImportVolume_triggered();

    void on_actionExportSeismic_triggered();

    void on_actionSaveProjectAs_triggered();

    void on_actionSave_triggered();

    void on_actionCloseProject_triggered();


    void on_actionCreateTimeslice_triggered();


    void on_actionHorizon_Semblance_triggered();

    void on_actionCompute_Intercept_Gradient_triggered();

    void updateProjectViews();


    void on_actionHorizon_Amplitudes_triggered();

    void on_actionCrossplot_Grids_triggered();

    void on_actionCrossplot_Volumes_triggered();

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


    void on_actionAmplitude_vs_Offset_Plot_triggered();

    void on_actionVolume_Amplitudes_triggered();

    void on_actionVolume_Semblance_triggered();

    void on_actionCompute_Intercept_and_Gradient_Volumes_triggered();

    void on_actionFluid_Factor_Grid_triggered();

    void on_actionSecondary_Attribute_Grids_triggered();

    void on_actionFluid_Factor_Volume_triggered();


    void on_action_Geometry_triggered();
    void on_actionAxis_Orientation_triggered();
    void on_actionDisplay_Map_triggered();


    void on_action_About_triggered();

#ifdef USE_KEYLOCK_LICENSE
    void on_licenseTimer();
    void on_checkLicense();
#endif



    void on_actionRun_Grid_User_Script_triggered();

    void on_actionRun_Volume_Script_triggered();



    void on_actionSecondary_Attribute_Volumes_triggered();

    void on_actionTrend_Based_Attribute_Grids_triggered();

    void on_actionTrend_Based_Attribute_Volumes_triggered();


protected:

    void closeEvent(QCloseEvent*);

private:

#ifdef USE_KEYLOCK_LICENSE
    license::LicenseInfo checkLicense();
#endif

    void setProjectFileName( const QString& fileName);
    bool openProject( const QString& fileName);
    bool saveProject( const QString& fileName);

    void displaySeismicDataset( const QString& name);
    void displaySeismicDatasetSlice( const QString& name);
    void displaySeismicDatasetIndex( const QString& name);
    void editSeismicDatasetProperties(const QString& name);
    void renameSeismicDataset(const QString&);
    void removeSeismicDataset(const QString&);
    void selectAndExportSeismicDataset();
    void exportSeismicDataset(QString);

    void displayGrid( GridType t, const QString&);
    void displayGridHistogram( GridType t, const QString&);
    void renameGrid( GridType t, const QString&);
    void removeGrid( GridType t, const QString&);
    void importGrid(GridType);
    void selectAndExportGrid(GridType);
    void exportGrid(GridType, QString);
    void runGridContextMenu(GridType, QListView*, const QPoint&);

    void displayVolume3D( const QString& );
    void displayVolumeSlice( const QString&);
    void displayVolumeRelativeSlice( const QString&);
    void displayVolumeHistogram( const QString&);
    void renameVolume( const QString&);
    void removeVolume( const QString&);
    void selectAndExportVolume();
    void exportVolume(QString);
    void exportVolumeSeisware(QString);

    void runProcess( ProjectProcess* process, QMap<QString, QString> params);

    void saveSettings();
    void loadSettings();

    void adjustForCurrentProject(const QString& filePath);
    void updateRecentProjectActionList();

    void updateMenu();

    Ui::ProjectViewer *ui;


    std::shared_ptr<AVOProject> m_project;
    QString m_projectFileName;
    std::shared_ptr<QLockFile> m_lockfile;

    std::shared_ptr<PointDispatcher> m_dispatcher;

    QStringList recentProjectFileList;
    QList<QAction*> recentProjectActionList;
    const int maxRecentProjectCount=5;

};

#endif // PROJECTVIEWER_H
