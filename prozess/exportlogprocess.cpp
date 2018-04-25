#include "exportlogprocess.h"

#include <avoproject.h>
#include <QApplication>
#include<iostream>
#include<fstream>
#include<wellpath.h>

ExportLogProcess::ExportLogProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Math"), project, parent){

}

ProjectProcess::ResultCode ExportLogProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    bool restrictDepths;
    double zmin, zmax;    
    try{
        m_wellName=getParam(parameters, "well");
        auto logs=getParam(parameters, "logs");
        m_logNames=unpackParamList(logs);
        m_outputPath=getParam(parameters, "output-file");
        m_nullStr=getParam(parameters, "null");
        restrictDepths=static_cast<bool>(getParam(parameters, "restrict-depths").toInt());
        zmin=getParam( parameters, "min-depth").toDouble();
        zmax=getParam( parameters, "max-depth").toDouble();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    if( restrictDepths ){
        m_zmin=zmin;
        m_zmax=zmax;
    }
    else{
        m_zmin=std::numeric_limits<double>::lowest();
        m_zmax=std::numeric_limits<double>::max();
    }

    return ResultCode::Ok;
}



ProjectProcess::ResultCode ExportLogProcess::run(){

    QVector<std::shared_ptr<Log>> logs;
    for( auto name : m_logNames){
        auto log=project()->loadLog( m_wellName, name);
        if( !log){
            setErrorString(QString("Loading log \"%1-%2\" failed!").arg(m_wellName, name));
            return ResultCode::Error;
        }
        logs.push_back(log);
    }

    for( int i=1; i<logs.size();i++){
        if( logs[i]->nz()!=logs[0]->nz()){
            setErrorString("Logs have different number of samples!");
            return ResultCode::Error;
        }
    }

    auto wellpath=project()->loadWellPath( m_wellName);

    std::ofstream os(m_outputPath.toStdString());
    if( !os.good()){
        setErrorString("Open output file failed!");
        return ResultCode::Error;
    }

    emit started(logs[0]->nz());
    
    // output header
    os<<"\"N\", \"X\", \"Y\", \"MD\", \"TVD\"";
    for( auto name : m_logNames){
        os<<", \""<<name.toStdString()<<"\"";
    }
    os<<std::endl;

    for( int i=0; i<logs[0]->nz(); i++){

        auto md=logs[0]->index2z(i);

        if( md>=m_zmin && md<=m_zmax ){

            QString tvdStr=m_nullStr;
            QString xStr=m_nullStr;
            QString yStr=m_nullStr;
            if(wellpath){
                auto tvd=wellpath->tvdAtMD(md);
                auto loc=wellpath->locationAtMD(md);
                xStr=QString::number(loc.x());
                yStr=QString::number(loc.y());
                tvdStr=QString::number(tvd);
            }

             os<<i+1<<", "<<xStr.toStdString()<<", "<<yStr.toStdString()<<", "<<md<<", "<<tvdStr.toStdString();

            for( int j=0; j<logs.size(); j++){

                auto log=logs[j];

                auto value=(*log)[i];
                QString valueStr;
                if( value!=log->NULL_VALUE){
                    valueStr=QString::number(value);
                }
                else{
                    valueStr=m_nullStr;
                }

                os<<", "<<valueStr.toStdString();
            }

            os<<std::endl;
        }
        

        emit progress(i);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}
