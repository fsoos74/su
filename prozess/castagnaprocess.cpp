#include "castagnaprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include<QMap>



CastagnaProcess::CastagnaProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Castagna Computation"), project, parent){

}

ProjectProcess::ResultCode CastagnaProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_wells=unpackParamList( getParam(parameters, "wells") );
        m_vpName=getParam(parameters, "vp-log");
        m_vsName=getParam(parameters, "vs-log");
        m_description=getParam(parameters,"description");
        m_a0=getParam(parameters, "a0").toDouble();
        m_a1=getParam(parameters, "a1").toDouble();
        m_a2=getParam(parameters, "a2").toDouble();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode CastagnaProcess::run(){

    emit started(m_wells.size());
    qApp->processEvents();

    int i=0;
    for( auto well : m_wells){

        auto res=processWell(well);
        if( res!=ResultCode::Ok) return res;

        emit progress(++i);
        qApp->processEvents();

        if( isCanceled()) return ResultCode::Canceled;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CastagnaProcess::processWell(QString well){

    auto vpLog=project()->loadLog( well, m_vpName);
    if( !vpLog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_vpName));
        return ResultCode::Error;
    }

    auto vsLog=std::make_shared<Log>( m_vsName.toStdString(), vpLog->unit(), m_description.toStdString(),
                                           vpLog->z0(), vpLog->dz(), vpLog->nz() );

    if( !vsLog ){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    for( int i=0; i<vpLog->nz(); i++){

        auto vp = (*vpLog)[i];
        auto vs=m_a2*std::pow(vp,2) + m_a1*vp + m_a0;
        (*vsLog)[i]=vs;
    }

    if( !project()->addLog( well, m_vsName, *vsLog ) ){
            setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(well, m_vsName) );
            return ResultCode::Error;
    }

    return ResultCode::Ok;
}
