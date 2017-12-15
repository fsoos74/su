#include "importgridprocess.h"

#include <avoproject.h>
#include <cmath>
#include <QApplication>
#include <QTextStream>

using namespace std::placeholders;


ImportGridProcess::ImportGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Import Grid"), project, parent){

}


ProjectProcess::ResultCode ImportGridProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_name=getParam( parameters, "name" );
        auto tstr=getParam( parameters, "type");
        m_type=toGridType(tstr);
        m_fileName=getParam(parameters, "file");
        auto fstr=getParam(parameters, "format");
        m_format=toGridFormat(fstr);
        m_nullValue=getParam( parameters, "null-value");
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


ProjectProcess::ResultCode ImportGridProcess::run(){


    QFile file( m_fileName );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( "Cannot open file for reading!");
        return ResultCode::Error;
    }

    QTextStream in(&file);
    int maxBytes=file.bytesAvailable();

    // scan file for il-xl range
    emit currentTask("Scanning grid geometry");
    emit started(maxBytes);

    int minil=std::numeric_limits<int>::max();
    int maxil=std::numeric_limits<int>::min();
    int minxl=std::numeric_limits<int>::max();
    int maxxl=std::numeric_limits<int>::min();
    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        int il;
        int xl;
        double value;
        if( processLine(line, il, xl, value)){
            //std::cout<<"il="<<il<<" xl="<<xl<<std::endl<<std::flush;
            if( il<minil) minil=il;
            if( il>maxil) maxil=il;
            if( xl<minxl) minxl=xl;
            if( xl>maxxl) maxxl=xl;
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
        if( isCanceled()){
            return ResultCode::Canceled;
        }
    }

    emit finished();
    qApp->processEvents();

    // allocate grid
    Grid2DBounds bounds(minil, minxl, maxil, maxxl);
    auto grid=std::make_shared<Grid2D<float>>(bounds);
    if( !grid ){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }


    // read file to grid
    emit currentTask("Importing values");
    emit started(maxBytes);

    in.resetStatus();
    in.seek(0);
    i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();

        if( i<=m_skipLines) continue;

        int il;
        int xl;
        double value;
        if( processLine(line, il, xl, value)){
            if(bounds.isInside(il,xl)) (*grid)(il,xl)=value;
        }

        emit progress(maxBytes-file.bytesAvailable());
        qApp->processEvents();
       // if( isCanceled()){
       //     return ResultCode::Canceled;
       // }
    }

    emit finished();
    qApp->processEvents();


    if( !project()->addGrid( m_type, m_name, grid, params())){
        setErrorString("Adding grid to project failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}



bool ImportGridProcess::processLine( const QString& line, int& il, int& xl, double& value){


     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn) return false;

     if( m_nullValue==col[m_valueColumn] ) return false;

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
