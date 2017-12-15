#include "flattenvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

FlattenVolumeProcess::FlattenVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Flatten Volume"), project, parent){

}

ProjectProcess::ResultCode FlattenVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_outputName=getParam(parameters, "output-volume");
        m_inputName=getParam(parameters, "input-volume");
        m_horizonName=getParam(parameters, "horizon" );
        m_length=getParam(parameters, "length").toInt();
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

    m_horizon=project()->loadGrid(GridType::Horizon, m_horizonName);
    if( !m_horizon){
        setErrorString(QString("Loading horizon \"%1\" failed!").arg(m_horizonName));
        return ResultCode::Error;
    }

    auto bounds=m_inputVolume->bounds();
    int out_nt = static_cast<int>(1 + 0.001*m_length / bounds.dt());  // length is in millis
    auto out_bounds=Grid3DBounds( bounds.i1(), bounds.i2(), bounds.j1(), bounds.j2(), out_nt, 0, bounds.dt());
    m_volume=std::shared_ptr<Volume >( new Volume(out_bounds));

    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}



ProjectProcess::ResultCode FlattenVolumeProcess::run(){


    Grid3DBounds bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            auto thms=m_horizon->valueAt(i,j);
            if( thms==m_horizon->NULL_VALUE) continue;

            auto th = 0.001*thms;

            for( int k=0; k<bounds.nt(); k++){
                (*m_volume)(i,j,k)=m_inputVolume->value( i, j, th + bounds.sampleToTime(k));
            }
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
