#include "avoproject.h"

#include<QDir>
#include<QFileInfo>
#include<QFile>
#include <QtSql/QtSql>
#include <QProgressDialog>
#include <QProgressBar>

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
#include<xppwriter.h>
#include<xppreader.h>
#include<xwiwriter.h>
#include<xwireader.h>
#include<log.h>
#include<logreader.h>
#include<logwriter.h>
#include<wellpath.h>
#include<wellpathwriter.h>
#include<wellpathreader.h>
#include<wellmarkers.h>
#include<topsdbmanager.h>


#include<iostream>
#include<fstream>
#include<iomanip>
#include<chrono>

/*
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
*/
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




AVOProject::AVOProject( QObject* parent) : QObject(parent)
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

    syncWellList();

    //emit changed();
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

 QString AVOProject::getGridPPPath(GridType t, const QString& gridName){

     return QDir::cleanPath( gridDirectory(t) + QDir::separator() + gridName + QString(".") + ProcessParamsSuffix );
 }

bool AVOProject::existsGrid( GridType t, const QString& name ){

    return m_gridLists.value(t).contains( name);   // XXX was !
}

bool AVOProject::addGrid( GridType t, const QString& name, std::shared_ptr<Grid2D<float>> grid, const ProcessParams& pp){

     if( name.isEmpty()) return false;

    if( m_gridLists.value(t).contains( name)) return false;

    if(!grid) return false;

    QString gridFileName=getGridPath( t, name);

    QFile file(gridFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    XGRWriter writer(*grid);
    if(!writer.writeFile(&file)) return false;
    file.flush();

    if( !pp.isEmpty() ){

        QString xpppath=getGridPPPath( t, name );
        QFile file(xpppath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        XPPWriter writer(pp);
        if( !writer.writeFile(&file) ) return false; // need clean up
        file.flush();
    }

    //syncGridList(t);

    m_gridLists[t].append(name);

    emit gridsChanged(t); //changed();

    return true;
}

bool AVOProject::removeGrid( GridType t, const QString& name){

    if( !gridList(t).contains(name)) return false;

    QString gridFileName=getGridPath( t, name);

    QFile file(gridFileName);

    bool res=file.remove();

    //syncGridList(t);
    m_gridLists[t].removeAll(name);
    emit gridsChanged(t);  //changed();

    return res;
}

bool AVOProject::renameGrid( GridType t, const QString& name, const QString& newName){

    if( !m_gridLists.value(t).contains(name)) return false;

    QString gridFileName=getGridPath(t, name);
    QString newGridFileName=getGridPath(t, newName);
    QFile file(gridFileName);
    bool res=file.rename(newGridFileName);

    //syncGridList(t);
    m_gridLists[t].removeAll(name);
    m_gridLists[t].append(newName);
    emit gridsChanged(t); //changed();

    return res;
}


Grid2DBounds AVOProject::getGridBounds( GridType type, const QString &name){

    QString filename=getGridPath( type, name);

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        throw std::runtime_error(QString("Could not open grid file %1").arg(filename).toStdString());
    }

    XGRReader reader;
    if( !reader.read(&file) ){
        throw std::runtime_error(QString("Reading grid file %1 failed!").arg(filename).toStdString());
    }

    return reader.bounds();
}


QFileInfo AVOProject::getGridFileInfo( GridType type, const QString& name ){
    QString path=getGridPath(type, name);
    return QFileInfo(path);
}


ProcessParams AVOProject::getGridProcessParams( GridType t, const QString &name){

    ProcessParams pp;

    QString pppath=getGridPPPath( t, name);
    QFile file(pppath);
    if( !file.open(QFile::ReadOnly | QFile::Text ) ){
        return pp; // return empy process params if no file for backard compability where no xpp
    }

    XPPReader reader(pp);

    if( !reader.readFile(&file) ){
        throw std::runtime_error(QString("Reading process parameters %1 failed!").arg(pppath).toStdString());
    }

    return pp;
}

 std::shared_ptr<Grid2D<float> > AVOProject::loadGrid(GridType t, const QString& name){

     QString gridFileName=getGridPath(t, name);

     std::shared_ptr<Grid2D<float> > ptr( new Grid2D<float>);

     QFile file(gridFileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         return ptr;
     }

     XGRReader reader(ptr.get());
     if( !reader.read(&file) ){
         qFatal("Reading grid failed!");
     }

     return ptr;
 }


bool AVOProject::saveGrid( GridType t, const QString& name, std::shared_ptr<Grid2D<float>> grid){

    Q_ASSERT( grid);
    // it is a serious error to call save on an not added grid
    Q_ASSERT( existsGrid(t,name));

    QString gridFileName=getGridPath( t, name);

    QFile file(gridFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    XGRWriter writer(*grid);
    auto res=writer.writeFile(&file);
    file.close();
    return res;
}

 QString AVOProject::getVolumePath( const QString& name){

     return QDir::cleanPath( m_volumeDirectory + QDir::separator() + name + QString(".") + VolumeSuffix);
 }

 QString AVOProject::getVolumePPPath( const QString& name){

     return QDir::cleanPath( m_volumeDirectory + QDir::separator() + name + QString(".") + ProcessParamsSuffix );
 }


bool AVOProject::existsVolume( const QString& name ){

    return m_volumeList.contains( name);
}

bool AVOProject::addVolume( const QString& name, std::shared_ptr<Volume> volume, const ProcessParams& pp){

    if( name.isEmpty()) return false;

    if( m_volumeList.contains( name)) return false;

    QString filename=getVolumePath( name);
    VolumeWriter writer(*volume);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;
    }
    ostr.close();

    if( !pp.isEmpty() ){

        QString xpppath=getVolumePPPath( name );
        QFile file(xpppath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        XPPWriter writer(pp);
        if( !writer.writeFile(&file) ) return false; // need clean up
        file.flush();
    }

    //syncVolumeList();
    m_volumeList.append(name);
    emit volumesChanged(); //changed();

    return true;
}

bool AVOProject::removeVolume( const QString& name){

    if( !volumeList().contains(name)) return false;

    QString filename=getVolumePath( name);
    QFile file(filename);
    bool res=file.remove();

    QFile ppfile(getVolumePPPath(name));
    ppfile.remove();

    //syncVolumeList();
    m_volumeList.removeAll(name);
    emit volumesChanged(); //changed();

    return res;
}

bool AVOProject::renameVolume( const QString& name, const QString& newName){

    if( !m_volumeList.contains(name)) return false;

    QString fileName=getVolumePath(name);
    QString newFileName=getVolumePath(newName);
    QFile file(fileName);
    bool res=file.rename(newFileName);

    QFile ppfile(getVolumePPPath(name));
    ppfile.rename(getVolumePPPath(newName));

    //syncVolumeList();
    m_volumeList.removeAll(name);
    m_volumeList.append(newName);
    emit volumesChanged(); //changed();

    return res;
}


 std::shared_ptr<Volume > AVOProject::loadVolume(const QString& name, bool showProgressDialog){

     QString filename=getVolumePath( name);

     std::shared_ptr<Volume > ptr( new Volume);

     VolumeReader reader(ptr.get());
     std::ifstream istr(filename.toStdString(), std::ios::in|std::ios::binary);

     QProgressDialog pdlg;
     if(showProgressDialog){
         pdlg.setWindowTitle(tr("Loading Volume"));
         pdlg.setLabelText(tr("Loading volume \"%1\"").arg(name));
         pdlg.setCancelButton(nullptr);
         pdlg.setRange(0,100);
         pdlg.setMinimumDuration(0);
         pdlg.show();
         connect( &reader, SIGNAL( percentDone(int)), &pdlg, SLOT(setValue(int)) );
     }

     if( !reader.readFromStream(istr)){
         std::shared_ptr<Volume>();
     }
     istr.close();

     return ptr;
 }

 std::shared_ptr<Volume > AVOProject::loadVolume(const QString& name, QProgressBar* pbar){

     QString filename=getVolumePath( name);

     std::shared_ptr<Volume > ptr( new Volume);

     VolumeReader reader(ptr.get());
     std::ifstream istr(filename.toStdString(), std::ios::in|std::ios::binary);

     if(pbar){
         pbar->setRange(0,100);
         connect( &reader, SIGNAL( percentDone(int)), pbar, SLOT(setValue(int)) );
     }

     if( !reader.readFromStream(istr)){
         std::shared_ptr<Volume>();
     }
     istr.close();

     return ptr;
 }



bool AVOProject::saveVolume(const QString &name, std::shared_ptr<Volume> volume){

    if( name.isEmpty()) return false;

    if( !volume) return false;

    if( !m_volumeList.contains( name)) return false;

    QString filename=getVolumePath( name);
    VolumeWriter writer(*volume);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;                   // XXX NEED TO HANDLE PARTIALLY WRITTEN/BROKEN VOLUMES!!!!
    }
    ostr.close();

    emit volumeChanged(name);

    return true;
}


Grid3DBounds AVOProject::getVolumeBounds(const QString &name, Domain* domain, VolumeType* type){

    QString filename=getVolumePath( name);

    VolumeReader reader;
    std::ifstream istr(filename.toStdString(), std::ios::in|std::ios::binary);
    if( !reader.readFromStream(istr)){
        throw std::runtime_error(QString("Could not open volume file %1").arg(filename).toStdString());
    }
    istr.close();

    if(domain) *domain=reader.domain();
    if(type) *type=reader.type();

    return reader.bounds();
}


QFileInfo AVOProject::getVolumeFileInfo( const QString& name ){
    QString path=getVolumePath(name);
    return QFileInfo(path);
}

ProcessParams AVOProject::getVolumeProcessParams(const QString &name){

    ProcessParams pp;

    QString pppath=getVolumePPPath(name);
    QFile file(pppath);
    if( !file.open(QFile::ReadOnly | QFile::Text ) ){
        return pp; // return empy process params if no file for backard compability where no xpp
    }

    XPPReader reader(pp);

    if( !reader.readFile(&file) ){
        std::cerr<<"Read faild:"<<reader.errorString().toStdString()<<std::endl<<std::flush;
        throw std::runtime_error(QString("Reading process parameters %1 failed!").arg(pppath).toStdString());
    }

    return pp;
}




QString AVOProject::getWellPath( const QString& name){

    return QDir::cleanPath( m_wellDirectory + QDir::separator() + name + QString(".") + WellSuffix);
}

WellInfo AVOProject::getWellInfo(const QString & name){

    WellInfo wellInfo;
    QString wellInfoPath=getWellPath(name);

    QFile wellInfoFile(wellInfoPath);
    if( !wellInfoFile.exists() || !wellInfoFile.open(QFile::ReadOnly | QFile::Text)){
        throw std::runtime_error("Cannot open well info file");
    }

    XWIReader wellInfoReader(wellInfo);
    if( !wellInfoReader.read(&wellInfoFile)){
        throw std::runtime_error("Reading well info file failed!");
    }

    return wellInfo;
}

bool AVOProject::setWellInfo(const QString &name, const WellInfo & info){

    if( !existsWell(name)) return false;

    QString xwiPath=getWellPath(name);
    QFile wellInfoFile(xwiPath);     // xwi
    if (!wellInfoFile.open(QFile::WriteOnly | QFile::Text)) return false;

    XWIWriter writer(info);
    if( ! writer.writeFile(&wellInfoFile) ){
        return false;
    }
    wellInfoFile.flush();        // THIS is VERY IMPORTANT, file content must be written before change is emitted!!!

    emit wellChanged(name); //changed();

    return true;
}

QFileInfo AVOProject::getWellFileInfo( const QString& name ){
    QString path=getWellPath(name);
    return QFileInfo(path);
}

bool AVOProject::existsWell( const QString& name ){

   return m_wellList.contains( name);
}

bool AVOProject::addWell(const QString &name, const WellInfo &wellInfo){

    if( name.isEmpty()) return false;

    QString xwiPath=getWellPath(name);
    QFile wellInfoFile(xwiPath);     // xwi
    if (!wellInfoFile.open(QFile::WriteOnly | QFile::Text)) return false;

    XWIWriter writer(wellInfo);
    if( ! writer.writeFile(&wellInfoFile) ){
        wellInfoFile.remove();                  // don't leave traces
        return false;
    }
    wellInfoFile.flush();        // THIS is VERY IMPORTANT, file content must be written before change is emitted!!!

    //create log directory for this well
    auto path=logDirectory(name);
    if(!QDir().mkpath(path)){
        removeWell(name);
        return false;
    }

    //syncWellList();
    m_wellList.append(name);
    emit wellsChanged(); //changed();

    return true;
}

void AVOProject::removeWell( const QString& name){

    if( !wellList().contains(name)) return;

    QString filename=getWellPath( name);
    QFile file(filename);
    file.remove();

    auto path=logDirectory(name);
    QDir(path).removeRecursively();     // potentially dangerous

    //syncWellList();
    m_wellList.removeAll(name);
    emit wellsChanged(); //changed();
}


QStringList AVOProject::logList(const QString &well){

    QDir dir(logDirectory(well));
    dir.setNameFilters(QStringList(QString("*.%1").arg(LogSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString wellName=QFileInfo(rawName).baseName();
        tmp.append(wellName);
    }

    return tmp;
}

QString AVOProject::logDirectory( const QString& well )const{
    return QDir::cleanPath( m_wellDirectory + QDir::separator() + well);
}

QString AVOProject::getLogPath( const QString& well, const QString& name ){
    return QDir::cleanPath( logDirectory(well) + QDir::separator() + name + QString(".") + LogSuffix);
}

bool AVOProject::existsLog(const QString& well, const QString& name){
    auto path=getLogPath(well, name);
    QFile file( path );
    return file.exists();
}

bool AVOProject::addLog( const QString& well, const QString& name, const Log& log){
    if( !existsWell(well)) return false;
    if( existsLog( well, name ) ) return false;

    auto filename=getLogPath(well, name);
    LogWriter writer(log);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;
    }

    emit wellChanged(well); //changed();

    return true;
}

bool AVOProject::saveLog(const QString &well, const QString &name, const Log &log){
    if( !existsWell(well)) return false;

    auto filename=getLogPath(well, name);
    LogWriter writer(log);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;
    }

    emit wellChanged(well); //changed();

    return true;
}

void AVOProject::removeLog( const QString& well, const QString& name){
    auto path=getLogPath(well, name);
    QFile file(path);
    file.remove();
    emit wellChanged(well); //changed();
}

std::shared_ptr<Log> AVOProject::loadLog( const QString& well, const QString& name ){

    if( !existsLog( well, name ) ) return std::shared_ptr<Log>();

    auto p=std::make_shared<Log>();

    auto path=getLogPath(well, name);

    LogReader reader(p.get());
    std::ifstream istr(path.toStdString(), std::ios::in|std::ios::binary);
    if( !reader.readFromStream(istr)){
        return std::shared_ptr<Log>();
    }

    return p;
}


QString AVOProject::getWellPathPath(const QString& well){
    return QDir::cleanPath( logDirectory(well) + QDir::separator() + well + QString(".") + WellPathSuffix );
}

bool AVOProject::existsWellPath(const QString& well){
    auto path=getWellPathPath(well);
    QFile file( path );
    return file.exists();
}

bool AVOProject::saveWellPath(const QString& well, const WellPath& path){
    if( !existsWell(well)) return false;

    auto filename=getWellPathPath(well);
    WellPathWriter writer(path);
    std::ofstream ostr(filename.toStdString(), std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        return false;
    }

    emit wellChanged(well); //changed();
    return true;
}

std::shared_ptr<WellPath> AVOProject::loadWellPath(const QString& well){
    if( !existsWellPath( well ) ) return std::shared_ptr<WellPath>();

    auto p=std::make_shared<WellPath>();

    auto path=getWellPathPath(well);

    WellPathReader reader(*p);
    std::ifstream istr(path.toStdString(), std::ios::in|std::ios::binary);
    if( !reader.readFromStream(istr)){
        return std::shared_ptr<WellPath>();
    }

    return p;
}


std::unique_ptr<TopsDBManager> AVOProject::openTopsDatabase(){
    auto path=getDatabasePath("tops");
    return std::unique_ptr<TopsDBManager>(new TopsDBManager(path));
}

/*
QString AVOProject::getWellMarkersPath(const QString& well){
    return QDir::cleanPath( logDirectory(well) + QDir::separator() + well + QString(".") + WellMarkersSuffix );
}

bool AVOProject::existsWellMarkers(const QString& well){
    auto path=getWellMarkersPath(well);
    QFile file( path );
    return file.exists();
}
*/
bool AVOProject::saveWellMarkers(const QString& well, const WellMarkers& wms){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;

    for( auto top : wms ){

        QString command=QString("select * from tops where name='%1' and uwi='%2'").arg(top.name(), top.uwi());

        if( !query.exec(command)){
            return false;
            //throw std::runtime_error(query.lastError().text().toStdString());
        }

        if( query.next()){  // record exits, update md
            command=QString("update tops set md=%1 where name='%2' and uwi='%3'").arg(QString::number(top.md()), top.name(), top.uwi());
        }
        else{               // new record
            command=QString("insert into tops (name, uwi, md) values('%1', '%2', %3)").arg(top.name(), top.uwi(), QString::number(top.md()) );
        }
        //std::cout<<command.toStdString()<<std::endl<<std::flush;
        if( !query.exec( command) ){
            return false;
                //throw std::runtime_error(query.lastError().text().toStdString());
        }
    }

    db.close();

    return true;
}

std::shared_ptr<WellMarkers> AVOProject::loadWellMarkersByWell(const QString& uwi){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    auto wms=std::make_shared<WellMarkers>();

    QSqlQuery query;
    QString command=QString("select name, md from tops where uwi='%1'").arg(uwi);
    if( query.exec(command)){
        while(query.next()){
            auto name=query.value(0).toString();
            auto md=query.value(1).toDouble();
            wms->push_back(WellMarker(name,uwi,md));
        }
    }

    db.close();

    return wms;
}


std::shared_ptr<WellMarkers> AVOProject::loadWellMarkersByName(const QString& name){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    auto wms=std::make_shared<WellMarkers>();

    QSqlQuery query;
    QString command=QString("select uwi, md from tops where name='%1'").arg(name);
    if( query.exec(command)){
        while(query.next()){
            auto uwi=query.value(0).toString();
            auto md=query.value(1).toDouble();
            wms->push_back(WellMarker(name,uwi,md));
        }
    }

    db.close();

    return wms;
}


void AVOProject::removeWellMarkersByWell(const QString& well){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;
    QString command=QString("delete from tops where uwi='%1'").arg(well);
    query.exec(command);

    db.close();
}

void AVOProject::removeWellMarkersByName(const QString& name){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;
    QString command=QString("delete from tops where name='%1'").arg(name);
    query.exec(command);

    db.close();
}



/*
    if( !existsWell(well)) return false;

    auto xwmpath=getWellMarkersPath(well);

    QFile file(xwmpath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    XWMWriter writer(wm);
    if( !writer.writeFile(&file) ) return false; // need clean up
    file.flush();

    emit wellChanged(well); //changed();
    return true;
}

std::shared_ptr<WellMarkers> AVOProject::loadWellMarkers(const QString& well){
    if( !existsWellMarkers( well ) ) return std::shared_ptr<WellMarkers>();

    auto p=std::make_shared<WellMarkers>();

    auto xwmpath=getWellMarkersPath(well);
    QFile file(xwmpath);
    if( !file.open(QFile::ReadOnly | QFile::Text ) ){
        return p; // return empy process params if no file for backard compability where no xpp
    }

    XWMReader reader(*p);

    if( !reader.readFile(&file) ){
        throw std::runtime_error(QString("Reading well markers %1 failed!").arg(xwmpath).toStdString());
    }

    return p;
}


bool AVOProject::saveWellMarkers(const QString& well, const WellMarkers& wm){

}
*/





 QString AVOProject::getSeismicDatasetPath(const QString& datasetName){

     return QDir::cleanPath( m_seismicDirectory + QDir::separator() + datasetName + QString(".") + SeismicDatasetSuffix);
 }

 QString AVOProject::getSeismicDatasetPPPath( const QString& name){

     return QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + ProcessParamsSuffix );
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

    datasetInfo.makeAbsolute(m_seismicDirectory);

    // handle old style dataset files, get sampling information from SEGY
    if( datasetInfo.ft()==0 && datasetInfo.dt()==0 && datasetInfo.nt()==0){
        QFile xsiFile(datasetInfo.infoPath());
        if( xsiFile.exists() && xsiFile.open(QFile::ReadOnly | QFile::Text)){
            seismic::SEGYInfo segyInfo;
            seismic::XSIReader xsiReader(segyInfo);
            xsiReader.read( &xsiFile );
            seismic::SEGYReader segyReader( datasetInfo.path().toStdString(), segyInfo);
            auto hdr=segyReader.read_trace_header();
            datasetInfo.setFT(0.001*hdr["delrt"].intValue());  // secs
            datasetInfo.setDT(segyReader.dt());
            datasetInfo.setNT(segyReader.nt());
        }
    }

    return datasetInfo;
}

QFileInfo AVOProject::getSeismicDatasetFileInfo( const QString& datasetName ){
    QString path=getSeismicDatasetInfo(datasetName).path();
    return QFileInfo(path);
}

ProcessParams AVOProject::getSeismicDatasetProcessParams(const QString &name){

    ProcessParams pp;

    QString pppath=getSeismicDatasetPPPath(name);
    QFile file(pppath);
    if( !file.open(QFile::ReadOnly | QFile::Text ) ){
        return pp; // return empy process params if no file for backard compability where no xpp
    }

    XPPReader reader(pp);

    if( !reader.readFile(&file) ){
        throw std::runtime_error(QString("Reading process parameters %1 failed!").arg(pppath).toStdString());
    }

    return pp;
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

    datasetInfoFile.flush();

    emit datasetChanged(datasetName);

    return true;
}

SeismicDatasetInfo AVOProject::genericDatasetInfo( const QString& name, SeismicDatasetInfo::Domain domain, SeismicDatasetInfo::Mode mode,
                                                   double ft, double dt, size_t nt){

    SeismicDatasetInfo info;

    info.setName(name);
    info.setPath( m_seismicDirectory + QDir::separator() + name + QString(".sgy") );
    info.setIndexPath( m_seismicDirectory + QDir::separator() + name + QString(".db3") );
    info.setInfoPath( m_seismicDirectory + QDir::separator() + name + QString(".xsi") );
    info.setDomain(domain);
    info.setMode(mode);
    info.setFT(ft);
    info.setDT(dt);
    info.setNT(nt);
    return info;
}



bool AVOProject::addSeismicDataset( const QString& name, const QString& segyPath, const seismic::SEGYInfo& segyInfo){

     if( name.isEmpty()) return false;

    QString segyInfoPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + SegyInfoSuffix);
    QString indexPath=QDir::cleanPath( m_seismicDirectory + QDir::separator() + name + QString(".") + DatabaseSuffix);

    // store segy info file
    QFile segyInfoFile(segyInfoPath);
    if (!segyInfoFile.open(QFile::WriteOnly | QFile::Text)) return false;
    seismic::XSIWriter segyInfoWriter(segyInfo);
    if( ! segyInfoWriter.writeFile(&segyInfoFile) ){
        segyInfoFile.remove();                  // don't leave traces
        return false;
    }
    //segyInfoFile.flush();
    segyInfoFile.close();

    SeismicDatasetInfo datasetInfo;
    datasetInfo.setName(name);

    // check if file is in seismic directory, i.e. imported, or attached
    if(QFileInfo(segyPath).absoluteDir().path() == m_seismicDirectory ){  // imported
        datasetInfo.setPath(QDir(m_seismicDirectory).relativeFilePath(segyPath));
    }
    else{   // attached
        datasetInfo.setPath(segyPath);
    }

    // segyinfo and index are always in seismic dir - make them relative
    datasetInfo.setInfoPath(QDir(m_seismicDirectory).relativeFilePath(segyInfoPath));
    datasetInfo.setIndexPath(QDir(m_seismicDirectory).relativeFilePath(indexPath));


    // create index file

    auto segyReader=std::make_shared<seismic::SEGYReader>(segyPath.toStdString(), segyInfo);
    if( !segyReader )return false;
    if( !createDatasetIndex(indexPath, segyReader) ) return false;
    segyReader.reset();  // close file
    return addSeismicDataset(name, datasetInfo, ProcessParams() );
}

bool AVOProject::addSeismicDataset( const QString& name, SeismicDatasetInfo info, const ProcessParams& pp ){

    QString xdiPath=getSeismicDatasetPath(name);
    QFile datasetInfoFile(xdiPath);     // xdi

    if( datasetInfoFile.exists() || !datasetInfoFile.open(QFile::WriteOnly | QFile::Text)){
        return false;
    }

    XDIWriter datasetInfoWriter(info);
    if( ! datasetInfoWriter.writeFile(&datasetInfoFile) ){
        datasetInfoFile.remove();                  // don't leave traces
        return false;
    }
    datasetInfoFile.flush();        // THIS is VERY IMPORTANT, file content must be written before change is emitted!!!

    if( !pp.isEmpty() ){

        QString xpppath=getSeismicDatasetPPPath( name );
        QFile file(xpppath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        XPPWriter writer(pp);
        if( !writer.writeFile(&file) ) return false; // need clean up
        file.flush();
    }

    //syncSeismicDatasetList();
    m_seismicDatasetList.append(name);
    emit datasetsChanged(); //changed();

    return true;
}

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

    //syncSeismicDatasetList();
    m_seismicDatasetList.removeAll(name);
    emit datasetsChanged(); // changed();

    return res;
}

bool AVOProject::renameSeismicDataset( const QString& name, const QString& newName){

    if( !m_seismicDatasetList.contains(name)) return false;

    QString datasetFileName=getSeismicDatasetPath(name);
    QString newDatasetFileName=getSeismicDatasetPath(newName);
    QFile file(datasetFileName);

    bool res=file.rename(newDatasetFileName);

    //syncSeismicDatasetList();
    m_seismicDatasetList.removeAll(name);
    m_seismicDatasetList.append(newName);
    emit datasetsChanged(); // changed();

    return res;
}

std::shared_ptr<SeismicDatasetReader> AVOProject::openSeismicDataset(const QString& datasetName){

    std::shared_ptr<SeismicDatasetReader> ptr;

    SeismicDatasetInfo datasetInfo=getSeismicDatasetInfo(datasetName);
    datasetInfo.makeAbsolute(seismicDirectory());

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

bool AVOProject::isAttachedSeismicDataset(const QString &datasetName){
    SeismicDatasetInfo datasetInfo=getSeismicDatasetInfo(datasetName);
    return QFileInfo(datasetInfo.path()).absolutePath()!=seismicDirectory();
}

bool copyCheckDifferent( const QString& source, const QString& dest ){

    std::cout<<"copyCheckDifferent"<<std::endl<<std::flush;
    std::cout<<"source: "<<source.toStdString()<<std::endl<<std::flush;
    std::cout<<"dest: "<<dest.toStdString()<<std::endl<<std::flush;
    if( QFile(dest).exists()){
        std::cout<<"dest exists"<<std::endl<<std::flush;
        return true;        // copy does not overwrite, MAYBE reomve old first?
    }

    if( QFileInfo(source).canonicalFilePath()==QFileInfo(dest).canonicalFilePath()){
        std::cout<<"source=dest"<<std::endl<<std::flush;
        return true;
    }

    QFile file(source);
    auto res=file.copy(dest);
    if( !res ){
        std::cout<<"COPY ERROR: "<<file.errorString().toStdString()<<std::endl<<std::flush;
    }

    return res;
}

bool AVOProject::importAttachedSeismicDataset(const QString &datasetName){

    SeismicDatasetInfo sourceDatasetInfo=getSeismicDatasetInfo(datasetName);
    //sourceDatasetInfo.makeAbsolute();

    auto newDatasetInfo=sourceDatasetInfo;
    newDatasetInfo.setPath( QFileInfo(sourceDatasetInfo.path()).fileName() );
    newDatasetInfo.setIndexPath( QFileInfo(sourceDatasetInfo.indexPath()).fileName() );
    newDatasetInfo.setInfoPath( QFileInfo(sourceDatasetInfo.infoPath()).fileName() );

    auto destDatasetInfo=newDatasetInfo;
    destDatasetInfo.makeAbsolute(seismicDirectory());

    // copy files if they are not already in the seismic dir
    int ok=copyCheckDifferent(sourceDatasetInfo.path(),destDatasetInfo.path());
    if( ok ) ok=copyCheckDifferent(sourceDatasetInfo.indexPath(),destDatasetInfo.indexPath());
    if( ok ) ok=copyCheckDifferent(sourceDatasetInfo.infoPath(),destDatasetInfo.infoPath());

    if( !ok ){
        // NEED TO REMOVE ALREADY COPIED FILES HERE !!!
        //QFile::remove(destDatasetInfo.path());
        //QFile::remove(destDatasetInfo.indexPath());
        //QFile::remove(destDatasetInfo.infoPath());
        std::cout<<"NOT OK"<<std::endl<<std::flush;
        return false;
    }

    setSeismicDatasetInfo( datasetName, newDatasetInfo );

    return true;
}

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
    const QString WellSubdir="well";

    m_seismicDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + SeismicSubdir);
    m_gridDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + GridSubdir);
    m_volumeDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + VolumeSubdir);
    m_databaseDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + DatabaseSubdir);
    m_wellDirectory=QDir::cleanPath( m_projectDirectory + QDir::separator() + WellSubdir );

    ensureDirectory(m_seismicDirectory);
    ensureDirectory(m_gridDirectory);
    ensureDirectory(m_volumeDirectory);
    ensureDirectory(m_databaseDirectory);
    ensureDirectory(m_wellDirectory);


    //for( auto type : GridTypesAndNames.keys() ){
    for( auto type : gridTypes() ){
        ensureDirectory(gridDirectory(type));
    }



}

void AVOProject::setupDatabases(){

    auto path=getDatabasePath("tops");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();
    QSqlQuery query;
    if( !query.exec("create table if not exists tops (name varchar(40), uwi varchar(20), md float, unique(name,uwi))") ){
        throw std::runtime_error(query.lastError().text().toStdString());
    }
    db.close();
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

    //for( auto type : GridTypesAndNames.keys() ){
    for( auto type : gridTypes() ){
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

    //for( auto type : GridTypesAndNames.keys()){
    for( auto type : gridTypes()){
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


void AVOProject::syncWellList(){

    QDir dir(m_wellDirectory);
    dir.setNameFilters(QStringList(QString("*.%1").arg(WellSuffix)));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList tmp;
    QStringList list = dir.entryList();
    for( QString rawName : list){

        QString wellName=QFileInfo(rawName).baseName();
        tmp.append(wellName);
    }

    m_wellList=tmp;
}
