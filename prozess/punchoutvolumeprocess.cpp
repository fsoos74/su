#include "punchoutvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

PunchOutVolumeProcess::PunchOutVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Punch Out Volume"), project, parent){

}

ProjectProcess::ResultCode PunchOutVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_outputName=getParam(parameters, "output-volume");
        m_inputName=getParam(parameters, "input-volume");
        m_tableName=getParam(parameters, "points-table");
        m_keepPoints=static_cast<bool>(getParam(parameters, "keep-points").toInt());
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_inputVolume=project()->loadVolume(m_inputName);
    if( !m_inputVolume ){
        setErrorString(QString("Loading volume \"%1\" failed!").arg(m_inputName));
        return ResultCode::Error;
    }

    m_table=project()->loadTable(m_tableName);
    if( !m_table ){
        setErrorString(QString("Loading table \"%1\" failed!").arg(m_tableName));
        return ResultCode::Error;
    }

    if( m_keepPoints){  // fill with null and fill later
        m_volume= std::shared_ptr<Volume >( new Volume(m_inputVolume->bounds()));
    }
    else{   // copy and null later
        m_volume=std::make_shared<Volume>(*m_inputVolume);
    }
    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode PunchOutVolumeProcess::run(){


    Grid3DBounds bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(bounds.i2()-bounds.i1());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            if( !m_table->contains(i,j)){
                continue;
            }

            auto times=m_table->values(i,j);

            if( m_keepPoints){
                for( auto t : times){
                    auto k=bounds.timeToSample(t);
                    (*m_volume)(i,j,k)=(*m_inputVolume)(i,j,k);
                }
            }else{
                for( auto t : times){
                    auto k=bounds.timeToSample(t);
                    (*m_volume)(i,j,k)=m_volume->NULL_VALUE;
                }
            }
        }

        emit progress(i-bounds.i1());
        qApp->processEvents();
    }

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
