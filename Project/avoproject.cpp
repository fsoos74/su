#include "avoproject.h"

#include<QDir>
#include<QFileInfo>
#include<QFile>
#include <QtSql/QtSql>
#include <QProgressDialog>

#include<grid2d.h>
#include<grid3d.h>
#include<xgrwriter.h>
#include<xgrreader.h>
#include<segyinfo.h>
#include<xsireader.h>
#include<xsiwriter.h>
#include"seismicdatasetinfo.h"
#include"xdiwriter.h"
#include"xdireader.h"
#include<volumereader.h>
#include<volumewriter.h>
#include<segyreader.h>
#include<seismicdatasetreader.h>

#include<iostream>
#include<fstream>
#include<iomanip>
#include<chrono>


//GridType string2GridType( const QString& str){
GridType toGridType( const QString& str){

   for( auto type : GridTypesAndNames.keys()){
       if( str == GridTypesAndNames.value(type) ){
           return type;
       }
   }

   throw std::runtime_error("Invalid grid name in string2GridType");
}

//QString gridType2String( GridType t ){
QString toQString( GridType t ){

    for( auto type : GridTypesAndNames.keys()){
        if( t == type ){
            return GridTypesAndNames.value(type);
        }
    }

    throw std::runtime_error("Invalid type in gridType2String");
}

const QString GridTypeAndNameDelimeter = "::";

QString createFullGridName( GridType type, QString name){

    return toQString(type) + GridTypeAndNameDelimeter + name;
}

std::pair<GridType, QString> splitFullGridName(const QString& fullName){

    QStringList parts=fullName.split(GridTypeAndNameDelimeter);
    if( parts.size()!=2){
        throw std::runtime_error("Invalid name string in splitFullGridName");
    }

    return std::pair<GridType, QString>( toGridType(parts[0]), parts[1]);

}




AVOProject::AVOProject()
{

}


void AVOProject::setGeometry(const ProjectGeometry &geom){

    m_geometry=geom;
}

void AVOProject::setInlineOrientation( const AxxisOrientation& o){
    m_inlineOrientation=o;
}

void AVOProject::setInlineDirection( const AxxisDirection& d){
    m_inlineDirection=d;
}

void AVOProject::setCrosslineDirection( const AxxisDirection& d){
    m_crosslineDirection=d;
}



void AVOProject::setProjectDirectory( const QString& dir){

    m_projectDirectory=dir;

    setupDirectories();

    setupDatabases();
    syncDatabaseList();

    setupGrids();
    syncGridLists();

    syncVolumeList();

    syncSeismicDatasetList();
}

QString AVOProject::getDatabasePath(const QString& databaseName){

    return QDir::cleanPath( m_databaseDirectory + QDir::separator() + databaseName + QString(".") + DatabaseSuffix);
}

/*
std::shared_ptr<CDPDatabase> AVOProject::cdpDatabase(){

    return std::shared_ptr<CDPDatabase>( new CDPDatabase(getDatabasePath("cdps")));
}
*/

 QString AVOProject::getGridPath( GridType t, const QString& gridName){

     return QDir::cleanPath( gridDirectory(t) + QDir::separator() + gridName + QString(".") + GridSuffix);
 }

bool AVOProject::existsGrid( GridType t, const QString& name ){

    return ! m_gridLists.value(t).contains( name);
}

bool AVOProject::addGrid( GridType t, const QString& name, std::shared_ptr<Grid2D<float>> grid){


    if( m_gridLists.value(t).contains( name)) return false;

    QString gridFileName=getGridPath( t, name);

    QFile file(gridFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    XGRWriter writer(*grid);
    writer.writeFile(&file);

    syncGridList(t);

    return true;
}

bool AVOProject::removeGrid( GridType t, const QString& name){

    if( !gridList(t).contains(name)) return false;

    QString gridFileName=getGridPath( t, name);

    QFile file(gridFileName);

    bool res=file.remove();

    syncGridList(t);

    return res;
}

bool AVOProject::renameGrid( GridType t, const QString& name, const QString& newName){

    if( !m_gridLists.value(t).contains(name)) return false;

    QString gridFileName=getGridPath(t, name);
    QString newGridFileName=getGridPath(t, newName);
    QFile file(gridFileName);
    bool res=file.rename(newGridFileName);

    syncGridList(t);

    return res;
}


 std::shared_ptr<Grid2D<float> > AVOProject::loadGrid(GridType t, const QString& name){

     QString gridFileName=getGridPath(t, name);

     std::shared_ptr<Grid2D<float> > ptr( new Grid2D<float>);

     QFile file(gridFileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         return ptr;
     }

     XGRReader reader(*ptr);
     if( !reader.read(&file) ){
         qFatal("Reading grid failed!");
     }

     return ptr;
 }





 QString AVOProject::getVolumePath( const QString& name){

     return QDir::cleanPath( m_volumeDirectory + QDir::separator() + name + QString(".") + VolumeSuffix);
 }

bool AVOProject::existsVolume( const QString& name ){

    return ! m_volumeList.contains( name);
}

bool AVOProject::addVolume( const QString& name, std::shared_ptr<Grid3D<float>> volume){


    if( m_volumeList.contains( name)) return false;

    QString filename=getVolumePath( name);

    VolumeWriter writer(*volume);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;
    }
    ostr.close();

    syncVolumeList();

    return true;
}

bool AVOProject::removeVolume( const QString& name){

    if( !volumeList().contains(name)) return false;

    QString filename=getVolumePath( name);

    QFile file(filename);

    bool res=file.remove();

    syncVolumeList();

    return res;
}

bool AVOProject::renameVolume( const QString& name, const QString& newName){

    if( !m_volumeList.contains(name)) return false;

    QString fileName=getVolumePath(name);
    QString newFileName=getVolumePath(newName);
    QFile file(fileName);
    bool res=file.rename(newFileName);

    syncVolumeList();

    return res;
}


 std::shared_ptr<Grid3D<float> > AVOProject::loadVolume(const QString& name){

     QString filename=getVolumePath( name);

     std::shared_ptr<Grid3D<float> > ptr( new Grid3D<float>);

     VolumeReader reader(*ptr);
     std::ifstream istr(filename.toStdString(), std::ios::in|std::ios::binary);
     if( !reader.readFromStream(istr)){
         std::shared_ptr<Grid3D<float>>();
     }
     istr.close();

     return ptr;
 }









 QString AVOProject::getSeismicDatasetPath(const QString& datasetName){

     return QDir::cleanPath( m_seismicDirectory + QDir::separator() + datasetName + QString(".") + SeismicDatasetSuffix);
 }


 // returns info as is, i.e. possibly relative paths, call makeAbsolute
SeismicDatasetInfo AVOProject::getSeismicDatasetInfo(const QString& datasetName){

    SeismicDatasetInfo datasetInfo;
    QString datasetInfoPath=getSeismicDatasetPath(datasetName);
    QFile datasetInfoFile(datasetInfoPath);
    if( !datasetInfoFile.exists() || !datasetInfoFile.open(QFile::ReadOnly | QFile::Text)){
        throw std::runtime_error("Cannot read dataset info file");
    }
    XDIReader datasetInfoReader(datasetInfo);
    if( !datasetInfoReader.read(&datasetInfoFile)){
        throw std::runtime_error("Reading dataset info file failed!");
    }

    /*
    // make relative paths absolute
    if( QDir(datasetInfo.infoPath()).isRelative() ){
        QString s=seismicDirectory() + QDir::separator() + datasetInfo.infoPath();
        datasetInfo.setInfoPath(s);
    }

    if( QDir(datasetInfo.indexPath()).isRelative()){
        QString s=seismicDirectory() + QDir::separator() + datasetInfo.indexPath();
        datasetInfo.setIndexPath(s);
    }

    if( QDir(datasetInfo.path()).isRelative()){
        QString s=seismicDirectory() + QDir::separator() + datasetInfo.path();
        datasetInfo.setPath(s);
    }
    */
/*
    std::cout<<"get dsinfo: "<<std::endl;
    std::cout<<"info path:"<<datasetInfo.infoPath().toStdString()<<std::endl;
    std::cout<<"index path:"<<datasetInfo.indexPath().toStdString()<<std::endl;
    std::cout<<"path:"<<datasetInfo.path().toStdString()<<std::endl<<std::flush;
*/
    return datasetInfo;
}

QStringList AVOProject::seismicDatasetList(SeismicDatasetInfo::Mode mode){

    QStringList list;

    for( QString name : m_seismicDatasetList ){

        if( getSeismicDatasetInfo(name).mode()==mode){
            list.append(name);
        }
    }

    return list;
}


bool AVOProject::setSeismicDatasetInfo( const QString& datasetName, const SeismicDatasetInfo& datasetInfo){

    if( !m_seismicDatasetList.contains(datasetName)) return false;

    QFile datasetInfoFile(getSeismicDatasetPath(datasetName));
    if( !datasetInfoFile.open(QFile::WriteOnly | QFile::Text))return false;

    XDIWriter datasetInfoWriter(datasetInfo);
    if( ! datasetInfoWriter.writeFile(&datasetInfoFile) ){
        return false;
    }
    return true;
}

bool AVOProject::addSeismicDataset( const QString& name, const QString& path, const seismic::SEGYInfo& segyInfo){

    QString relSegyInfoPath=name + ".xsi";
    QString segyInfoPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + relSegyInfoPath);
    QFile segyInfoFile(segyInfoPath);
    if (!segyInfoFile.open(QFile::WriteOnly | QFile::Text)) return false;
    seismic::XSIWriter segyInfoWriter(segyInfo);
    if( ! segyInfoWriter.writeFile(&segyInfoFile) ){
        segyInfoFile.remove();                  // don't leave traces
        return false;
    }

    QString relIndexPath=name + QString(".") + DatabaseSuffix;
    QString indexPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + relIndexPath);

    SeismicDatasetInfo datasetInfo;
    datasetInfo.setName(name);

    // copy path
    QString opath=path;
    // check if segfile is already in seismic dir
    if(QFileInfo(path).absoluteDir().path() == m_seismicDirectory ){
        // make path relative
        opath=QDir(m_seismicDirectory).relativeFilePath(opath);
        std::cout<<"MADE RELATIVE: "<<opath.toStdString()<<std::endl;
    }

    datasetInfo.setPath(opath); // segy
    datasetInfo.setInfoPath(relSegyInfoPath);
    datasetInfo.setIndexPath(relIndexPath);
    QString xdiPath=getSeismicDatasetPath(name);

    QFile datasetInfoFile(xdiPath);     // xdi

    if( datasetInfoFile.exists() || !datasetInfoFile.open(QFile::WriteOnly | QFile::Text)){
        segyInfoFile.remove();
        return false;
    }

    std::shared_ptr<seismic::SEGYReader> ptr;

    ptr=std::shared_ptr<seismic::SEGYReader>(new seismic::SEGYReader(path.toStdString(), segyInfo) );  // we need the absolute path here
    if( !ptr || !createDatasetIndex(indexPath, ptr) ) return false;                                     // needabs path


    XDIWriter datasetInfoWriter(datasetInfo);
    if( ! datasetInfoWriter.writeFile(&datasetInfoFile) ){
        segyInfoFile.remove();
        datasetInfoFile.remove();                  // don't leave traces
        QFile(datasetInfo.indexPath()).remove();
        return false;
    }

    syncSeismicDatasetList();

    return true;
}


/*
 * bool AVOProject::addSeismicDataset( const QString& name, const QString& path, const seismic::SEGYInfo& segyInfo){

    QString segyInfoPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + "xsi");
    QFile segyInfoFile(segyInfoPath);
    if (!segyInfoFile.open(QFile::WriteOnly | QFile::Text)) return false;
    seismic::XSIWriter segyInfoWriter(segyInfo);
    if( ! segyInfoWriter.writeFile(&segyInfoFile) ){
        segyInfoFile.remove();                  // don't leave traces
        return false;
    }

    QString indexPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + DatabaseSuffix);

    SeismicDatasetInfo datasetInfo;
    datasetInfo.setName(name);
    datasetInfo.setPath(path);
    datasetInfo.setInfoPath(segyInfoPath);
    datasetInfo.setIndexPath(indexPath);
    QFile datasetInfoFile(getSeismicDatasetPath(name));
    if( datasetInfoFile.exists() || !datasetInfoFile.open(QFile::WriteOnly | QFile::Text)){
        segyInfoFile.remove();
        return false;
    }

    std::shared_ptr<seismic::SEGYReader> ptr;

    ptr=std::shared_ptr<seismic::SEGYReader>(new seismic::SEGYReader(datasetInfo.path().toStdString(), segyInfo) );
    if( !ptr || !createDatasetIndex(datasetInfo.indexPath(), ptr) ) return false;


    XDIWriter datasetInfoWriter(datasetInfo);
    if( ! datasetInfoWriter.writeFile(&datasetInfoFile) ){
        segyInfoFile.remove();
        datasetInfoFile.remove();                  // don't leave traces
        QFile(datasetInfo.indexPath()).remove();
        return false;
    }

    syncSeismicDatasetList();

    return true;
}
*/

bool AVOProject::removeSeismicDataset( const QString& name){

    if( !seismicDatasetList().contains(name)) return false;

    SeismicDatasetInfo info=getSeismicDatasetInfo(name);

    QString infoPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + "xdi");
    QFile infoFile(infoPath);
    infoFile.remove();

    QFile segyInfoFile(info.infoPath());
    segyInfoFile.remove();

    QFile indexFile(info.indexPath());
    indexFile.remove();

    QFile file(info.path());
    bool res=file.remove();

    syncSeismicDatasetList();

    return res;
}

bool AVOProject::renameSeismicDataset( const QString& name, const QString& newName){

    if( !m_seismicDatasetList.contains(name)) return false;

    QString datasetFileName=getSeismicDatasetPath(name);
    QString newDatasetFileName=getSeismicDatasetPath(newName);
    QFile file(datasetFileName);

    bool res=file.rename(newDatasetFileName);

    syncSeismicDatasetList();

    return res;
}

std::shared_ptr<SeismicDatasetReader> AVOProject::openSeismicDataset(const QString& datasetName){

    std::shared_ptr<SeismicDatasetReader> ptr;

    SeismicDatasetInfo datasetInfo=getSeismicDatasetInfo(datasetName);
    datasetInfo.makeAbsolute(seismicDirectory());
    //QString datasetInfoPath=getSeismicDatasetPath(datasetName);
    //QFile datasetInfoFile(datasetInfoPath);
    //if( !datasetInfoFile.exists() || !datasetInfoFile.open(QFile::ReadOnly | QFile::Text)) return ptr;  // return nullptr on error
    //XDIReader datasetInfoReader(datasetInfo);
    //if( !datasetInfoReader.read(&datasetInfoFile)) return ptr;  // return nullptr on error

    std::shared_ptr<SeismicDatasetReader> reader;

    try{
        reader=std::shared_ptr<SeismicDatasetReader>( new SeismicDatasetReader(datasetInfo));
    }
    catch(seismic::SEGYFormatError& err){
        std::cerr<<"Could not open SEGYReader: "<<err.what()<<std::endl;
    };

    return reader;
}

/*
 * std::shared_ptr<SeismicDatasetReader> AVOProject::openSeismicDataset(const QString& datasetName){

    std::shared_ptr<SeismicDatasetReader> ptr;

    SeismicDatasetInfo datasetInfo;
    QString datasetInfoPath=getSeismicDatasetPath(datasetName);
    QFile datasetInfoFile(datasetInfoPath);
    if( !datasetInfoFile.exists() || !datasetInfoFile.open(QFile::ReadOnly | QFile::Text)) return ptr;  // return nullptr on error
    XDIReader datasetInfoReader(datasetInfo);
    if( !datasetInfoReader.read(&datasetInfoFile)) return ptr;  // return nullptr on error

    return std::shared_ptr<SeismicDatasetReader>( new SeismicDatasetReader(datasetInfo));
}
*/

/*
std::shared_ptr<seismic::SEGYReader> AVOProject::openSeismicDataset(const QString& datasetName){

    std::shared_ptr<seismic::SEGYReader> ptr;

    SeismicDatasetInfo datasetInfo;
    QString datasetInfoPath=getSeismicDatasetPath(datasetName);
    QFile datasetInfoFile(datasetInfoPath);
    if( !datasetInfoFile.exists() || !datasetInfoFile.open(QFile::ReadOnly | QFile::Text)) return ptr;  // return nullptr on error
    XDIReader datasetInfoReader(datasetInfo);
    if( !datasetInfoReader.read(&datasetInfoFile)) return ptr;  // return nullptr on error

    seismic::SEGYInfo segyInfo;
    QFile segyInfoFile(datasetInfo.infoPath());
    if( !segyInfoFile.exists() || !segyInfoFile.open(QFile::ReadOnly | QFile::Text)) return ptr;  // return nullptr on error
    seismic::XSIReader segyInfoReader(segyInfo);
    if( !segyInfoReader.read(&segyInfoFile)) return ptr;  // return nullptr on error
    ptr=std::shared_ptr<seismic::SEGYReader>(new seismic::SEGYReader(datasetInfo.path().toStdString(), segyInfo) );

    return ptr;
}
*/

QString AVOProject::getSeismicDatasetIndexPath(const QString& datasetName){

    SeismicDatasetInfo datasetInfo;
    QString datasetInfoPath=getSeismicDatasetPath(datasetName);
    QFile datasetInfoFile(datasetInfoPath);
    if( !datasetInfoFile.exists() || !datasetInfoFile.open(QFile::ReadOnly | QFile::Text)) return QString::null;  // return nullptr on error
    XDIReader datasetInfoReader(datasetInfo);
    if( !datasetInfoReader.read(&datasetInfoFile)) return QString::null;
    return datasetInfo.indexPath();
}

bool AVOProject::createDatasetIndex( const QString& path, std::shared_ptr<seismic::SEGYReader> reader){

    QString tableName="map";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery queryCreateTable( QString("CREATE TABLE IF NOT EXISTS %1 ("
            "trace integer primary key, cdp integer,  iline  integer, xline  integer, x  real,  y real, offset real"
            ")").arg(tableName) );
    if( !queryCreateTable.exec()) return false;// qFatal(QString(QString("Create table: ")+queryCreateTable.lastError().text()).toStdString().c_str());


     auto start = std::chrono::steady_clock::now();

    db.transaction();


    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 (trace, cdp, iline, xline, x, y, offset) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?)").arg(tableName));

    ssize_t ntrc=reader->trace_count();
    QProgressDialog* pdlg=new QProgressDialog("Creating Dataset Index", "Cancel", 0, ntrc, 0);


    for( ssize_t i=0; i<ntrc; i++){

        reader->seek_trace(i);
        seismic::Header hdr=reader->read_trace_header();

        query.addBindValue(int(i));
        query.addBindValue(int(hdr.at("cdp").intValue()));
        query.addBindValue(int(hdr.at("iline").intValue()));
        query.addBindValue(int(hdr.at("xline").intValue()));
        query.addBindValue(hdr.at("cdpx").floatValue());
        query.addBindValue(hdr.at("cdpy").floatValue());
        query.addBindValue(hdr.at("offset").floatValue());


        if( !query.exec()) return false;// qFatal(QString(QString("CDPDatabase put: ")+query.lastError().text()).toStdString().c_str());

        pdlg->setValue(i+1);
        qApp->processEvents();
        if( pdlg->wasCanceled()){
            pdlg->setValue(ntrc);
            db.rollback();
            return false;   // must remove traces!!!!
        }
    }

    db.commit();

    db.close();

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;


    return true;
}



void AVOProject::ensureDirectory(const QString& dirName){

    if( ! QDir().mkpath(dirName) ){
        throw PathException(QString("Failed to create directory \"%1\"").arg(dirName) );
    }

    if( !QFileInfo(dirName).isDir()){
        throw PathException(QString("Path \"%1\" exists but is not a directory").arg(dirName) );
    }

}

void AVOProject::setupDirectories(){

    const QString SeismicSubdir="seismic";
    const QString GridSubdir="grids";
    const QString VolumeSubdir="volumes";
    const QString DatabaseSubdir="db";


    m_seismicDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + SeismicSubdir);
    m_gridDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + GridSubdir);
    m_volumeDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + VolumeSubdir);
    m_databaseDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + DatabaseSubdir);

    ensureDirectory(m_seismicDirectory);
    ensureDirectory(m_gridDirectory);
    ensureDirectory(m_volumeDirectory);
    ensureDirectory(m_databaseDirectory);

    for( auto type : GridTypesAndNames.keys() ){
        ensureDirectory(gridDirectory(type));
    }

}

void AVOProject::setupDatabases(){

//    CDPDatabase db(getDatabasePath("cdps"));
}

void AVOProject::syncDatabaseList(){


    QDir dir(m_databaseDirectory);
    dir.setNameFilters(QStringList(QString("*.%1").arg(DatabaseSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString dbName=QFileInfo(rawName).baseName();
        tmp.append(dbName);
    }

    m_databaseList=tmp;
}


void AVOProject::setupGrids(){

    for( auto type : GridTypesAndNames.keys() ){
        m_gridLists.insert( type, QStringList() );

    }
}

QString AVOProject::gridDirectory( GridType t)const{

    QString sectionName = toQString( t );

    return m_gridDirectory + QDir::separator() + sectionName;
}

QStringList AVOProject::gridList( GridType t)const{

    if( !m_gridLists.contains(t)){
        qFatal("Invalid grid section type when trying to access grid list");

    }
    return m_gridLists.value(t);
}


void AVOProject::syncGridLists(){

    for( auto type : GridTypesAndNames.keys()){
        syncGridList(type);
    }
}

void AVOProject::syncGridList( GridType t){


    QDir dir( gridDirectory(t) );
    dir.setNameFilters(QStringList(QString("*.%1").arg(GridSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString gridName=QFileInfo(rawName).baseName();
        tmp.append(gridName);
    }

    m_gridLists.insert(t, tmp);
}

void AVOProject::syncVolumeList(){


    QDir dir(m_volumeDirectory);
    dir.setNameFilters(QStringList(QString("*.%1").arg(VolumeSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString datasetName=QFileInfo(rawName).baseName();
        tmp.append(datasetName);
    }

    m_volumeList=tmp;
}


void AVOProject::syncSeismicDatasetList(){


    QDir dir(m_seismicDirectory);
    dir.setNameFilters(QStringList(QString("*.%1").arg(SeismicDatasetSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString datasetName=QFileInfo(rawName).baseName();
        tmp.append(datasetName);
    }

    m_seismicDatasetList=tmp;
}


