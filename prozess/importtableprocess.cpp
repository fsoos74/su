#include "importtableprocess.h"

#include <avoproject.h>
#include <cmath>
#include <QApplication>
#include <QTextStream>

using namespace std::placeholders;


ImportTableProcess::ImportTableProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Import Grid"), project, parent){

}


ProjectProcess::ResultCode ImportTableProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_name=getParam( parameters, "name" );
        m_fileName=getParam(parameters, "file");
        auto fstr=getParam(parameters, "format");
        m_format=toGridFormat(fstr);
        m_skipLines=getParam(parameters, "skip-lines").toInt();
        m_coord1Column=getParam(parameters, "coord1-column").toInt() - 1; // make 0-based
        m_coord2Column=getParam(parameters, "coord2-column").toInt() - 1; // make 0-based
        m_valueColumn=getParam(parameters, "value-column").toInt()   - 1; // make 0-based
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_lastUsedColumn = std::max( std::max(m_coord1Column, m_coord2Column), m_valueColumn);

    switch( m_format){
        case GridFormat::XYZ:{
            project()->geometry().computeTransforms(m_c1c2_to_ilxl, m_ilxl_to_c1c2);
            break;
        }
        case GridFormat::ILXLZ:
            m_c1c2_to_ilxl = QTransform();  // id
            m_ilxl_to_c1c2 = QTransform();  // id
            break;
        default:
            setErrorString("Unhandled grid format!");
            return ResultCode::Error;
        break;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode ImportTableProcess::run(){


    QFile file( m_fileName );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( "Cannot open file for reading!");
        return ResultCode::Error;
    }

    QTextStream in(&file);
    int maxBytes=file.bytesAvailable();

    Table table("iline","xline",true);  // import all tables as multiple values per cdp, make this selectable

    emit currentTask("Importing values");
    emit started(maxBytes);

    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        int il;
        int xl;
        double value;
        if( processLine(line, il, xl, value)){
            table.insert(il, xl, value);
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
        if( isCanceled()){
            return ResultCode::Canceled;
        }
    }

    emit finished();
    qApp->processEvents();


    if( !project()->addTable( m_name, table) ){
        setErrorString("Adding table to project failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}



bool ImportTableProcess::processLine( const QString& line, int& il, int& xl, double& value){


     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn) return false;

     bool ok;
     auto coord1=col[m_coord1Column].toDouble(&ok);
     if( !ok ) return false;

     auto coord2=col[m_coord2Column].toDouble(&ok);
     if( !ok ) return false;

     value=col[m_valueColumn].toDouble(&ok);
     if( !ok ) return false;

     auto pilxl = m_c1c2_to_ilxl.map(QPointF(coord1, coord2));
     il=static_cast<int>(std::round(pilxl.x()));
     xl=static_cast<int>(std::round(pilxl.y()));

     return true;
}
