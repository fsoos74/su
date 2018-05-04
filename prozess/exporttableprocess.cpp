#include "exporttableprocess.h"

#include <avoproject.h>
#include <fstream>
#include <cstdio>

#include <QApplication>

using namespace std::placeholders;


ExportTableProcess::ExportTableProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Export Table"), project, parent){

}


ProjectProcess::ResultCode ExportTableProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    project()->geometry().computeTransforms(m_xy_to_ilxl, m_ilxl_to_xy);

    QString  tname;
    GridFormat format;
    try{
       m_outputName=getParam( parameters, QString("output-file") );
       tname=getParam( parameters, QString("table") );
       QString fstr=getParam( parameters, "format");
       format=toGridFormat(fstr);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    switch( format ){
    case GridFormat::XYZ:
        m_formatLineFunction=std::bind( &ExportTableProcess::formatLineXYZ, this, _1, _2, _3); break;
    case GridFormat::ILXLZ:
        m_formatLineFunction=std::bind( &ExportTableProcess::formatLineILXLZ, this, _1, _2, _3); break;
    case GridFormat::XYILXLZ:
        m_formatLineFunction=std::bind( &ExportTableProcess::formatLineXYILXLZ, this, _1, _2, _3); break;
    default:
        setErrorString("Unhandled Format");
        return ResultCode::Error;
    }

    //load input table
    m_inputTable=project()->loadTable( tname);
    if( !m_inputTable ){
        setErrorString("Loading table failed!");
        return ResultCode::Error;
    }


    return ResultCode::Ok;
}


ProjectProcess::ResultCode ExportTableProcess::run(){

    std::ofstream os(m_outputName.toStdString());
    if( !os.good()){
        setErrorString("Open output file failed!");
        return ResultCode::Error;
    }

    emit currentTask("Writing Table");
    emit started(m_inputTable->size());
    qApp->processEvents();

    // this assumes table is iline/xline/value, check for other tables, if no iline and xline also disable formats with xy

    int n=0;
    for( auto it = m_inputTable->cbegin(); it!=m_inputTable->cend(); it++){
        auto k12=m_inputTable->split_key( it.key() );
        auto il=k12.first;
        auto xl=k12.second;
        auto z=it.value();
        auto line=m_formatLineFunction(il,xl,z);
        os<<line.toStdString()<<std::endl;
        n++;
        emit progress(n);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return (os.good()) ? ResultCode::Ok : ResultCode::Error;
}


QString ExportTableProcess::zToQString(double z){

    return QString::number(z);
}

QString ExportTableProcess::formatLineXYZ(int il, int xl, double z){

    QPointF xy=m_ilxl_to_xy.map(QPoint(il, xl));
    QString line;
    line.sprintf( "%10.1lf %10.1lf ", xy.x(), xy.y() );
    line.append( zToQString(z));
    return line;
}

QString ExportTableProcess::formatLineILXLZ(int il, int xl, double z){

    QString line;
    line.sprintf( "%5d %5d ", il, xl );
    line.append( zToQString(z));
    return line;
}

QString ExportTableProcess::formatLineXYILXLZ(int il, int xl, double z){

    QString line;
    QPointF xy=m_ilxl_to_xy.map(QPoint(il, xl));
    //line=QString("%1 %2 %3 %4 %5").arg(QString::number(xy.x(),'f',1), QString::number(xy.y(),'f',1),
    //                                  QString::number(il), QString::number(xl), zToQString(z));
    line.sprintf( "%10.1lf %10.1lf %5d %5d ", xy.x(), xy.y(), il, xl);
    line.append( zToQString(z) );
    return line;
}
