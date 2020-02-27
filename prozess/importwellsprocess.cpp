#include "importwellsprocess.h"

#include <avoproject.h>
#include <QApplication>
#include <QTextStream>

using namespace std::placeholders;


ImportWellsProcess::ImportWellsProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Import Wells"), project, parent){

}


ProjectProcess::ResultCode ImportWellsProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_fileName=getParam(parameters, "file");
        m_skipLines=getParam(parameters, "skip-lines").toInt();
        m_nameColumn=getParam(parameters, "name-column").toInt() - 1; // make 0-based
        m_uwiColumn=getParam(parameters, "uwi-column").toInt() - 1; // make 0-based
        m_xColumn=getParam(parameters, "x-column").toInt() - 1; // make 0-based
        m_yColumn=getParam(parameters, "y-column").toInt() - 1; // make 0-based
        m_zColumn=getParam(parameters, "z-column").toInt() - 1; // make 0-based
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_lastUsedColumn = std::max(m_nameColumn, std::max(m_uwiColumn, std::max(m_xColumn, std::max(m_yColumn, m_zColumn))));


    return ResultCode::Ok;
}


ProjectProcess::ResultCode ImportWellsProcess::run(){


    QFile file( m_fileName );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( "Cannot open file for reading!");
        return ResultCode::Error;
    }

    QTextStream in(&file);
    int maxBytes=file.bytesAvailable();

    emit currentTask("Importing wells");
    emit started(maxBytes);

    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        WellInfo wellInfo;
        if( processLine(line, wellInfo)){
            if(!project()->addWell(wellInfo.name(), wellInfo)){
                setErrorString(tr("Adding well \"%1\" failed!").arg(wellInfo.name()));
                return ResultCode::Error;
            }
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
        if( isCanceled()){
            return ResultCode::Canceled;
        }
    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}



bool ImportWellsProcess::processLine( const QString& line, WellInfo& wellInfo){


     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn) return false;

     bool ok;
     wellInfo.setName(col[m_nameColumn]);
     wellInfo.setUWI(col[m_uwiColumn]);
     wellInfo.setX(col[m_xColumn].toDouble(&ok));
     if( !ok ) return false;
     wellInfo.setY(col[m_yColumn].toDouble(&ok));
     if( !ok ) return false;
     wellInfo.setZ(col[m_zColumn].toDouble(&ok));
     if( !ok ) return false;


     return true;
}
