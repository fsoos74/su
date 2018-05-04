#include "exportgridprocess.h"

#include <avoproject.h>
#include <fstream>
#include <cstdio>

#include <QApplication>

using namespace std::placeholders;


ExportGridProcess::ExportGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Export Grid"), project, parent){

}


ProjectProcess::ResultCode ExportGridProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    project()->geometry().computeTransforms(m_xy_to_ilxl, m_ilxl_to_xy);

    GridType gtype;
    QString  gname;
    GridFormat format;
    try{
       m_outputName=getParam( parameters, QString("output-file") );
       gname=getParam( parameters, QString("input-name") );
       QString tstr=getParam( parameters, "input-type");
       gtype=toGridType(tstr);
       m_null_value=getParam( parameters, "null-value");
       QString fstr=getParam( parameters, "format");
       format=toGridFormat(fstr);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    switch( format ){
    case GridFormat::XYZ:
        m_formatLineFunction=std::bind( &ExportGridProcess::formatLineXYZ, this, _1, _2, _3); break;
    case GridFormat::ILXLZ:
        m_formatLineFunction=std::bind( &ExportGridProcess::formatLineILXLZ, this, _1, _2, _3); break;
    case GridFormat::XYILXLZ:
        m_formatLineFunction=std::bind( &ExportGridProcess::formatLineXYILXLZ, this, _1, _2, _3); break;
    default:
        setErrorString("Unhandled Format");
        return ResultCode::Error;
    }

    //load input grid
    m_inputGrid=project()->loadGrid( gtype, gname);
    if( !m_inputGrid ){
        setErrorString("Loading grid failed!");
        return ResultCode::Error;
    }


    return ResultCode::Ok;
}


ProjectProcess::ResultCode ExportGridProcess::run(){

    std::ofstream os(m_outputName.toStdString());
    if( !os.good()){
        setErrorString("Open output file failed!");
        return ResultCode::Error;
    }

    Grid2DBounds bounds=m_inputGrid->bounds();

    emit currentTask("Iterating cdps");
    emit started(bounds.height());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            auto z=(*m_inputGrid)(i,j);
            auto line=m_formatLineFunction(i,j,z);
            os<<line.toStdString()<<std::endl;
        }

        emit progress(i-bounds.i1());
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return (os.good()) ? ResultCode::Ok : ResultCode::Error;
}


QString ExportGridProcess::zToQString(double z){

    if( z!=m_inputGrid->NULL_VALUE){
        return QString::number(z);
    }
    else{
        return m_null_value;
    }
}

QString ExportGridProcess::formatLineXYZ(int il, int xl, double z){

    QPointF xy=m_ilxl_to_xy.map(QPoint(il, xl));
    QString line;
    line.sprintf( "%10.1lf %10.1lf ", xy.x(), xy.y() );
    line.append( zToQString(z));
    return line;
}

QString ExportGridProcess::formatLineILXLZ(int il, int xl, double z){

    QString line;
    line.sprintf( "%5d %5d ", il, xl );
    line.append( zToQString(z));
    return line;
}

QString ExportGridProcess::formatLineXYILXLZ(int il, int xl, double z){

    QString line;
    QPointF xy=m_ilxl_to_xy.map(QPoint(il, xl));
    //line=QString("%1 %2 %3 %4 %5").arg(QString::number(xy.x(),'f',1), QString::number(xy.y(),'f',1),
    //                                  QString::number(il), QString::number(xl), zToQString(z));
    line.sprintf( "%10.1lf %10.1lf %5d %5d ", xy.x(), xy.y(), il, xl);
    line.append( zToQString(z) );
    return line;
}
