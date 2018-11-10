#include "copylogprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<wellpath.h>

CopyLogProcess::CopyLogProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Copy Log"), project, parent){

}

ProjectProcess::ResultCode CopyLogProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);


    try{
        m_sourceWell=getParam(parameters, "source-well");
        m_sourceLog=getParam(parameters, "source-log");
        m_destinationWell=getParam(parameters, "destination-well");
        m_destinationLog=getParam(parameters, "destination-log");
        m_description=getParam(parameters, "description");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }
    return ResultCode::Ok;
}


ProjectProcess::ResultCode CopyLogProcess::run(){

    auto slog=project()->loadLog(m_sourceWell,m_sourceLog);
    auto spath=project()->loadWellPath(m_sourceWell);
    auto dpath=project()->loadWellPath(m_destinationWell);
    auto tvd0=spath->tvdAtMD(slog->z0());
    auto tvd1=spath->tvdAtMD(slog->lz());
    auto z0=dpath->mdAtTVD(tvd0);
    auto z1=dpath->mdAtTVD(tvd1);
    auto dz=slog->dz();
    auto nz=static_cast<int>(std::ceil((z1-z0)/dz));
    Log dlog(m_destinationLog.toStdString(),slog->unit(),m_description.toStdString(),z0,dz,nz );

    for( int i=0; i<nz; i++){
        auto md=dlog.index2z(i);
        auto tvd=dpath->tvdAtMD(md);
        auto mds=spath->mdAtTVD(tvd);
        auto value=(*slog)(mds);
        dlog[i]=value;
    }

    project()->addLog(m_destinationWell, m_destinationLog, dlog);
    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}
