#ifndef AVOPROJECT_H
#define AVOPROJECT_H

#include<QObject>
#include<QString>
#include<QVector>
#include<QStringList>
#include<QMap>
#include<QDir>
#include<QFile>
#include<QFileInfo>
#include<QLockFile>
#include<stdexcept>
#include<memory>
#include<iostream>
class QProgressBar;

#include "projectgeometry.h"
#include "axxisorientation.h"
#include<grid2d.h>
#include<grid3d.h>
#include<volume.h>
#include"seismicdatasetinfo.h"
#include"wellinfo.h"
#include<processparams.h>
#include<gridtype.h>
#include <wellmarkers.h>

class WellPath;
class CDPDatebase;
class Log;
class TopsDBManager;
class Table;


namespace seismic{
class SEGYInfo;
class SEGYReader;
}

class SeismicDatasetReader;

QString createFullGridName( GridType, QString );
std::pair<GridType, QString> splitFullGridName(const QString&);

class AVOProject : public QObject
{
    Q_OBJECT

public:

    QString GridSuffix="xgr";
    QString DatabaseSuffix="db3";
    QString SeismicDatasetSuffix="xdi";
    QString SegyInfoSuffix="xsi";
    QString VolumeSuffix="vol";
    QString ProcessParamsSuffix="xpp";
    QString WellSuffix="xwi";
    QString LogSuffix="log";
    QString WellPathSuffix="wpt";
    QString TableSuffix="btb";

    class PathException : public std::runtime_error{
    public:
        PathException(const QString& msg ):std::runtime_error(msg.toStdString()){
        }
    };


    AVOProject( QObject* parent=0 );

    QString lockfileName()const{
        return QDir(m_projectDirectory).absoluteFilePath("lock.lock");
    }

    const ProjectGeometry& geometry()const{
        return m_geometry;
    }

    void setGeometry( const ProjectGeometry&);

    const AxxisOrientation& inlineOrientation()const{
        return m_inlineOrientation;
    }

    void setInlineOrientation( const AxxisOrientation&);

    const AxxisDirection& inlineDirection()const{
        return m_inlineDirection;
    }

    void setInlineDirection( const AxxisDirection&);

    const AxxisDirection& crosslineDirection()const{
        return m_crosslineDirection;
    }

    void setCrosslineDirection( const AxxisDirection&);


    const QString& projectDirectory()const{
        return m_projectDirectory;
    }

    void setProjectDirectory( const QString&);



    QString getDatabasePath( const QString& dbName);

    const QString& databaseDirectory()const{
        return m_databaseDirectory;
    }

    const QStringList& databaseList()const{
        return m_databaseList;
    }

    //std::shared_ptr<CDPDatabase> cdpDatabase();


    // grids

    QString gridDirectory(GridType)const;
    QStringList gridList(GridType)const;
    QString getGridPath(GridType, const QString& gridName);
    QString getGridPPPath(GridType, const QString& gridName);
    Grid2DBounds getGridBounds( GridType, const QString& name);
    QFileInfo getGridFileInfo( GridType, const QString& name );
    ProcessParams getGridProcessParams( GridType, const QString& name);
    std::shared_ptr<Grid2D<float>> loadGrid(GridType, const QString& name);
    bool saveGrid( GridType, const QString&, std::shared_ptr<Grid2D<float>>);
    bool existsGrid( GridType, const QString& name );
    bool addGrid( GridType, const QString& name, std::shared_ptr<Grid2D<float>> grid, const ProcessParams& params=ProcessParams());
    bool removeGrid( GridType, const QString& name);
    bool renameGrid( GridType, const QString& name, const QString& newName);


    // Volumes

    QString volumeDirectory()const;
    QStringList volumeList()const{
        return m_volumeList;
    }
    QString getVolumePath( const QString& volumeName);
    QString getVolumePPPath(const QString& name);
    std::shared_ptr<Volume> loadVolume(const QString& name, bool showProgessDialog);
    std::shared_ptr<Volume> loadVolume(const QString& name, QProgressBar* progressBar=nullptr);
    bool saveVolume(const QString& name, std::shared_ptr<Volume>);
    Grid3DBounds getVolumeBounds(const QString& name, Domain* domain=nullptr, VolumeType* type=nullptr);
    QFileInfo getVolumeFileInfo( const QString& name );
    ProcessParams getVolumeProcessParams(const QString& name);
    bool existsVolume( const QString& name );
    bool addVolume( const QString& name, std::shared_ptr<Volume> volume, const ProcessParams& params=ProcessParams()); // enforced only during refactor
    bool removeVolume( const QString& name);
    bool renameVolume( const QString& name, const QString& newName);


    // Wells

    const QString& wellDirectory()const{
        return m_wellDirectory;
    }
    const QStringList& wellList()const{
        return m_wellList;
    }
    QString getWellPath( const QString&);
    WellInfo getWellInfo( const QString&);
    bool setWellInfo( const QString& name, const WellInfo&);
    QFileInfo getWellFileInfo( const QString&);
    bool existsWell( const QString& );
    bool addWell( const QString& name, const WellInfo&);
    void removeWell( const QString& name);
    QStringList logList( const QString& well);
    QString logDirectory( const QString& well )const;
    QString getLogPath( const QString& well, const QString& name );
    bool existsLog(const QString& well, const QString& name );
    bool addLog( const QString& well, const QString& name, const Log&);
    bool saveLog( const QString& well, const QString& name, const Log&);
    void removeLog( const QString& well, const QString& name);
    std::shared_ptr<Log> loadLog( const QString& well, const QString& name );
    QString getWellPathPath(const QString& well);
    bool existsWellPath(const QString& well);
    bool saveWellPath(const QString& well, const WellPath& path);
    std::shared_ptr<WellPath> loadWellPath(const QString& well);
    std::unique_ptr<TopsDBManager> openTopsDatabase();
    bool saveWellMarkers(const QString& well, const WellMarkers& wm);
    std::shared_ptr<WellMarkers> loadWellMarkersByWell(const QString& name);
    std::shared_ptr<WellMarkers> loadWellMarkersByName(const QString& well);
    void removeWellMarkersByWell(const QString& name);
    void removeWellMarkersByName(const QString& name);


    // tables

    QString tablesDirectory()const{
        return m_tableDirectory;
    }
    QStringList tableList()const{
        return m_tableList;
    }
    QString getTablePath( const QString& name );
    QFileInfo getTableFileInfo( const QString&);
    std::tuple<QString, QString, bool, int> getTableInfo(QString name);
    bool existsTable(const QString& name);
    bool addTable( const QString& name, const Table& table);
    bool removeTable( const QString& name);
    bool renameTable( const QString& name, const QString& newName);
    std::shared_ptr<Table> loadTable(const QString& name);
    bool saveTable(const QString& name, const Table& table);


    // Seismic Datasets

    const QString& seismicDirectory()const{
        return m_seismicDirectory;
    }
    const QStringList& seismicDatasetList()const{
        return m_seismicDatasetList;
    }
    QStringList seismicDatasetList(SeismicDatasetInfo::Mode);
    SeismicDatasetInfo getSeismicDatasetInfo( const QString& datasetName);
    QFileInfo getSeismicDatasetFileInfo( const QString& datasetName );
    ProcessParams getSeismicDatasetProcessParams(const QString& name);
    QString getSeismicDatasetPPPath( const QString& name);
    bool setSeismicDatasetInfo( const QString& datasetName, const SeismicDatasetInfo& info);
    SeismicDatasetInfo genericDatasetInfo( const QString& name,
                                           int dimensions, SeismicDatasetInfo::Domain domain, SeismicDatasetInfo::Mode mode,
                                           double ft, double dt, size_t nt);
    bool addSeismicDataset( const QString& name, int dimensions, SeismicDatasetInfo::Domain domain, SeismicDatasetInfo::Mode mode,
                            const QString& path, const seismic::SEGYInfo& info);  // from SEGY
    bool addSeismicDataset( const QString& name, SeismicDatasetInfo info, const ProcessParams& pp=ProcessParams() );
    bool removeSeismicDataset( const QString& name);
    bool renameSeismicDataset( const QString& name, const QString& newName);
    std::shared_ptr<SeismicDatasetReader> openSeismicDataset(const QString& datasetName);
    bool isAttachedSeismicDataset(const QString& datasetName);
    bool importAttachedSeismicDataset( const QString& datasetName);

signals:
    //void changed();
    void geometryChanged();
    void datasetsChanged();
    void datasetChanged(QString);
    void gridsChanged(GridType);
    void gridChanged(GridType, QString);
    void volumesChanged();
    void volumeChanged(QString);
    void wellsChanged();
    void wellChanged(QString name);
    void tablesChanged();
    void tableChanged(QString name);

private:
    // made these private because external access must be through datasetinfo which handles relative paths!!!!
    QString getSeismicDatasetPath(const QString& datasetName);
    QString getSeismicDatasetIndexPath(const QString& datasetName);


private:

    bool createDatasetIndex( const QString& path, std::shared_ptr<seismic::SEGYReader>, int dims, bool prestack );

    void ensureDirectory(const QString&);
    void setupDirectories();
    void setupDatabases();
    void syncDatabaseList();
    void setupGrids();
    void syncGridLists();
    void syncGridList(GridType);
    void syncVolumeList();
    void syncSeismicDatasetList();
    void syncWellList();
    void syncTableList();

    ProjectGeometry m_geometry;

    AxxisOrientation m_inlineOrientation=AxxisOrientation::Horizontal;
    AxxisDirection m_inlineDirection=AxxisDirection::Ascending;
    AxxisDirection m_crosslineDirection=AxxisDirection::Ascending;

    QString m_projectDirectory;
    QString m_seismicDirectory;
    QString m_gridDirectory;
    QString m_volumeDirectory;
    QString m_databaseDirectory;
    QString m_wellDirectory;
    QString m_tableDirectory;

    QStringList m_databaseList;
    QMap<GridType, QStringList> m_gridLists;
    QStringList m_volumeList;
    QStringList m_seismicDatasetList;
    QStringList m_wellList;
    QStringList m_tableList;
};

#endif // AVOPROJECT_H
