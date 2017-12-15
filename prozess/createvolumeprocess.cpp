#include "createvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

CreateVolumeProcess::CreateVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Create Volume"), project, parent){

}


ProjectProcess::ResultCode CreateVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    int minZ;
    int maxZ;
    int dz;
    Domain domain;
    VolumeType type;

    try{

        m_outputName=getParam(parameters, "volume");
        minIline=getParam(parameters, "min-iline").toInt();
        maxIline=getParam( parameters, "max-iline").toInt();
        minXline=getParam( parameters, "min-xline").toInt();
        maxXline=getParam( parameters, "max-xline").toInt();
        minZ=getParam( parameters, "min-z").toInt();
        maxZ=getParam( parameters, "max-z").toInt();
        dz=getParam( parameters, "dz").toInt();
        auto domain_s=getParam( parameters, "domain");
        domain=toDomain(domain_s);
        auto type_s=getParam( parameters, "type");
        type=toVolumeType(type_s);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    int nz=1 + ( maxZ - minZ )/dz;
    Grid3DBounds bounds(minIline, maxIline, minXline, maxXline, nz, 0.001*minZ, 0.001*dz);
    m_volume=std::shared_ptr<Volume >( new Volume(bounds));
    m_volume->setDomain(domain);
    m_volume->setType(type);

    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CreateVolumeProcess::run(){

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
