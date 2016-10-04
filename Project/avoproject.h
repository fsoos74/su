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

#include "projectgeometry.h"
#include "axxisorientation.h"
#include<grid2d.h>
#include<grid3d.h>
#include"seismicdatasetinfo.h"
//#include<cdpdatabase.h>


class CDPDatebase;

namespace seismic{
class SEGYInfo;
class SEGYReader;
}

class SeismicDatasetReader;


enum class GridType{ Horizon, Attribute, Other };

static const QMap<GridType, QString> GridTypesAndNames{
    {GridType::Horizon, "Horizon"},
    {GridType::Attribute, "Attribute"},
    {GridType::Other, "Other"}
};

GridType string2GridType( const QString& );
QString gridType2String( GridType );

QString createFullGridName( GridType, QString );
std::pair<GridType, QString> splitFullGridName(const QString&);

class AVOProject //: public QObject
{
    //Q_OBJECT

public:

    QString GridSuffix="xgr";
    QString DatabaseSuffix="db3";
    QString SeismicDatasetSuffix="xdi";
    QString VolumeSuffix="vol";


    class PathException : public std::runtime_error{
    public:
        PathException(const QString& msg ):std::runtime_error(msg.toStdString()){
        }
    };


    AVOProject();

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


    QString gridDirectory(GridType)const;

    QStringList gridList(GridType)const;

    QString getGridPath(GridType, const QString& gridName);

    std::shared_ptr<Grid2D<float>> loadGrid(GridType, const QString& name);

    bool existsGrid( GridType, const QString& name );

    bool addGrid( GridType, const QString& name, std::shared_ptr<Grid2D<float>> grid);

    bool removeGrid( GridType, const QString& name);

    bool renameGrid( GridType, const QString& name, const QString& newName);


    QString volumeDirectory()const;

    QStringList volumeList()const{
        return m_volumeList;
    }

    QString getVolumePath( const QString& volumeName);

    std::shared_ptr<Grid3D<float>> loadVolume(const QString& name);

    bool existsVolume( const QString& name );

    bool addVolume( const QString& name, std::shared_ptr<Grid3D<float>> volume);

    bool removeVolume( const QString& name);

    bool renameVolume( const QString& name, const QString& newName);



    const QString& seismicDirectory()const{
        return m_seismicDirectory;
    }

    const QStringList& seismicDatasetList()const{
        return m_seismicDatasetList;
    }

    QStringList seismicDatasetList(SeismicDatasetInfo::Mode);

    SeismicDatasetInfo getSeismicDatasetInfo( const QString& datasetName);

    bool setSeismicDatasetInfo( const QString& datasetName, const SeismicDatasetInfo& info);

    bool addSeismicDataset( const QString& name, const QString& path, const seismic::SEGYInfo& info);

    bool removeSeismicDataset( const QString& name);

    bool renameSeismicDataset( const QString& name, const QString& newName);

    std::shared_ptr<SeismicDatasetReader> openSeismicDataset(const QString& datasetName);
    //std::shared_ptr<seismic::SEGYReader> openSeismicDataset(const QString& datasetName);


private:
    // made these private because external access must be through datasetinfo which handles relative paths!!!!
    QString getSeismicDatasetPath(const QString& datasetName);
    QString getSeismicDatasetIndexPath(const QString& datasetName);


private:

    bool createDatasetIndex( const QString& path, std::shared_ptr<seismic::SEGYReader> );

    void ensureDirectory(const QString&);
    void setupDirectories();
    void setupDatabases();
    void syncDatabaseList();
    void setupGrids();
    void syncGridLists();
    void syncGridList(GridType);
    void syncVolumeList();
    void syncSeismicDatasetList();


    ProjectGeometry m_geometry;

    AxxisOrientation m_inlineOrientation=AxxisOrientation::Horizontal;
    AxxisDirection m_inlineDirection=AxxisDirection::Ascending;
    AxxisDirection m_crosslineDirection=AxxisDirection::Ascending;

    QString m_projectDirectory;
    QString m_seismicDirectory;
    QString m_gridDirectory;
    QString m_volumeDirectory;
    QString m_databaseDirectory;

    QStringList m_databaseList;
    QMap<GridType, QStringList> m_gridLists;
    QStringList m_volumeList;
    QStringList m_seismicDatasetList;
};

#endif // AVOPROJECT_H
