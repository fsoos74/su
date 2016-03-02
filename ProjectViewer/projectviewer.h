#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include <QMainWindow>
#include <QString>
#include <avoproject.h>
#include<memory>
#include<QVector>
#include<QPoint>

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

    void on_actionSaveProjectAs_triggered();

    void on_actionSave_triggered();


    void on_actionCreateTimeslice_triggered();


    void on_actionHorizon_Semblance_triggered();

    void on_actionCompute_Intercept_Gradient_triggered();

    void updateProjectViews();


    void on_actionHorizon_Amplitudes_triggered();

    void on_action_Crossplot_triggered();

    void runDatasetContextMenu(const QPoint& pos);

    void runHorizonContextMenu(const QPoint& pos);
    void runAttributeContextMenu(const QPoint& pos);
    void runOtherGridContextMenu(const QPoint& pos);

    void runVolumeContextMenu(const QPoint& pos);


    void on_actionAmplitude_vs_Offset_Plot_triggered();

    void on_actionCompute_Intercept_and_Gradient_Volumes_triggered();

    void on_actionFluid_Factor_Grid_triggered();

    void on_actionFluid_Factor_Volume_triggered();


    void on_actionExportVolume_triggered();

    void on_actionImportVolume_triggered();

    void on_action_Geometry_triggered();

    void on_action_About_triggered();

#ifdef USE_KEYLOCK_LICENSE
    void on_licenseTimer();
    void on_checkLicense();
#endif

protected:

    void closeEvent(QCloseEvent*);

private:

#ifdef USE_KEYLOCK_LICENSE
    license::LicenseInfo checkLicense();
#endif
    void setProjectFileName( const QString& fileName);
    bool saveProject( const QString& fileName);

    void displaySeismicDataset( const QString& name);
    void displaySeismicDatasetIndex( const QString& name);
    void editSeismicDatasetProperties(const QString& name);
    void renameSeismicDataset(const QString&);
    void removeSeismicDataset(const QString&);

    void displayGrid( GridType t, const QString&);
    void displayGridHistogram( GridType t, const QString&);
    void renameGrid( GridType t, const QString&);
    void removeGrid( GridType t, const QString&);
    void importGrid(GridType);
    void selectAndExportGrid(GridType);
    void exportGrid(GridType, QString);
    void runGridContextMenu(GridType, QListView*, const QPoint&);

    void renameVolume( const QString&);
    void removeVolume( const QString&);
    void selectAndExportVolume();
    void exportVolume(QString);

    void runProcess( ProjectProcess* process, QMap<QString, QString> params);

    void saveSettings();
    void loadSettings();

    void updateMenu();

    Ui::ProjectViewer *ui;


    std::shared_ptr<AVOProject> m_project;
    QString m_projectFileName;

    std::shared_ptr<PointDispatcher> m_dispatcher;

};

#endif // PROJECTVIEWER_H
