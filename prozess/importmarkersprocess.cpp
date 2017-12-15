#include "importmarkersprocess.h"

#include <avoproject.h>
#include<topsdbmanager.h>
#include <QApplication>
#include <QTextStream>
#include <iostream>



ImportMarkersProcess::ImportMarkersProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Import Well Markers"), project, parent){

}


ProjectProcess::ResultCode ImportMarkersProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_fileName=getParam(parameters, "file");
        m_skipLines=getParam(parameters, "skip-lines").toInt();
        m_separator=getParam(parameters, "separator");
        m_uwiColumn=getParam(parameters, "uwi-column").toInt() - 1; // make 0-based
        m_nameColumn=getParam(parameters, "name-column").toInt() - 1; // make 0-based
        m_mdColumn=getParam(parameters, "md-column").toInt()   - 1; // make 0-based
        m_replace=static_cast<bool>(getParam(parameters, "replace").toInt());
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_lastUsedColumn = std::max( std::max(m_uwiColumn, m_nameColumn), m_mdColumn);

    return ResultCode::Ok;
}


ProjectProcess::ResultCode ImportMarkersProcess::run(){

    auto dbmgr=project()->openTopsDatabase();

    try{

    QFile file( m_fileName );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( "Cannot open file for reading!");
        return ResultCode::Error;
    }

    QTextStream in(&file);
    int maxBytes=file.bytesAvailable();

    emit currentTask("Importing markers");
    emit started(maxBytes);

    int i=0;
    std::vector<WellMarker> buf;
    const size_t CHUNK_SIZE = 1024;
    buf.reserve(CHUNK_SIZE);

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        QString uwi;
        QString name;
        double md;
        if( processLine(line, uwi, name, md)){
            //dbmgr->set(WellMarker(name,uwi,  md), m_replace);
            //dbmgr->insert(WellMarker(name,uwi,  md));
            buf.push_back(WellMarker(name,uwi,  md));
            if( buf.size()>=CHUNK_SIZE){
                dbmgr->insert(buf.begin(), buf.end());
                //dbmgr->replace(buf.begin(), buf.end());
                buf.clear();
            }
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
        if( isCanceled()){
            return ResultCode::Canceled;
        }
    }

    dbmgr->insert(buf.begin(), buf.begin()+buf.size());
    //dbmgr->replace(buf.begin(), buf.begin()+buf.size());

    emit finished();
    qApp->processEvents();

    }catch(std::exception& ex){
        setErrorString(QString("Exception occured: %1").arg(ex.what()));
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


/*
ProjectProcess::ResultCode ImportMarkersProcess::run(){


    QFile file( m_fileName );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( "Cannot open file for reading!");
        return ResultCode::Error;
    }

    QTextStream in(&file);
    int maxBytes=file.bytesAvailable();

    emit currentTask("Importing markers");
    emit started(maxBytes);

    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        QString uwi;
        QString name;
        double md;
        if( processLine(line, uwi, name, md)){
            if( !m_markers.contains(uwi) ){
                m_markers.insert(uwi, WellMarkers());
            }
            m_markers[uwi].push_back(WellMarker(name,uwi,  md));
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
        if( isCanceled()){
            return ResultCode::Canceled;
        }
    }

    auto uwis=m_markers.keys();
    emit currentTask("Adding markers to project");
    emit started(uwis.size());

    i=0;
    for( auto uwi : uwis ){

        if( !project()->existsWell(uwi)) continue;

        auto projectMarkers = project()->loadWellMarkersByWell(uwi); // returns null pointer ifwell does not exist, otherwise returns at least empty vector
        QMap<QString, WellMarker> tmp;
        if(  projectMarkers){
            for( auto pm : *projectMarkers){
                tmp.insert(pm.name(), pm);
            }
        }

        for( auto m : m_markers.value(uwi)){
            if( tmp.contains(m.name()) && !m_replace) continue;
            tmp[m.name()]=m;
        }

        WellMarkers wms;
        for( auto name : tmp.keys()){
            wms.push_back(tmp.value(name));
        }

        if( !project()->saveWellMarkers(uwi, wms) ){
            setErrorString(QString("Saving markers for well \"%1\ failed!").arg(uwi));
            return ResultCode::Error;
        }

        emit progress(++i);
        qApp->processEvents();
    }


    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}
*/



bool ImportMarkersProcess::processLine( const QString& line, QString& uwi, QString& name, double& md){


     QRegExp sep(m_separator);// "(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn) return false;

     uwi=col[m_uwiColumn];
     if( uwi.isEmpty()) return false;
     //uwi.replace("'","|");

     name=col[m_nameColumn];
     if( name.isEmpty()) return false;
     //uwi.replace("'","|");

     bool ok=true;
     md=col[m_mdColumn].toDouble(&ok);
     if( !ok ) return false;

     return true;
}
