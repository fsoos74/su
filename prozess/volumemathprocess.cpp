#include "volumemathprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include<functional>
using namespace std::placeholders;

VolumeMathProcess::VolumeMathProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Math"), project, parent){

}

ProjectProcess::ResultCode VolumeMathProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    QString iname1;
    QString iname2;
    double value=0;
    QString topName;
    QString bottomName;

    try{
        func=getParam(parameters, "function");
        m_outputName=getParam(parameters, "output-volume");
        iname1=getParam(parameters, "input-volume1");
        iname2=getParam(parameters, "input-volume2");
        value=getParam(parameters, "value").toDouble();
        topName=getParam( parameters, "top-horizon");
        bottomName=getParam( parameters, "bottom-horizon");
        m_editMode=static_cast<bool>(getParam(parameters, "edit-mode").toInt());
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    int nv=(iname2.isEmpty()) ? 1 :  2;
    emit currentTask("Loading Volumes");
    emit started(nv);
    qApp->processEvents();
    m_inputVolume1=project()->loadVolume(iname1);
    if( !m_inputVolume1){
        setErrorString(QString("Loading input volume 1 failed!"));
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();


    if( !iname2.isEmpty()){
        m_inputVolume2=project()->loadVolume(iname2);
        if( !m_inputVolume2){
            setErrorString(QString("Loading input volume 2 failed!"));
            return ResultCode::Error;
        }
        emit progress(2);
        qApp->processEvents();
    }

    emit currentTask("Loading Horizons");
    emit started(2);
    qApp->processEvents();
    if(topName!="NONE"){
        m_topHorizon=project()->loadGrid(GridType::Horizon, topName);
        if(!m_topHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(topName));
            return ResultCode::Error;
        }
    }
    emit progress(1);
    qApp->processEvents();

    if(bottomName!="NONE"){
        m_bottomHorizon=project()->loadGrid(GridType::Horizon, bottomName);
        if(!m_bottomHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(bottomName));
            return ResultCode::Error;
        }
    }
    emit progress(2);
    qApp->processEvents();

    m_outputVolume=std::make_shared<Volume>(m_inputVolume1->bounds());

    if( !m_outputVolume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    m_processor.setOP(MathProcessor::toOP(func));
    m_processor.setValue(value);
    m_processor.setInputNullValue(m_inputVolume1->NULL_VALUE);

    return ResultCode::Ok;
}


ProjectProcess::ResultCode VolumeMathProcess::run(){


    if( m_editMode){
        std::copy( m_inputVolume1->cbegin(), m_inputVolume1->cend(), m_outputVolume->begin());
    }


    Grid3DBounds bounds=m_outputVolume->bounds();

    emit currentTask("Computing output volume");
    emit started(bounds.ni());
    qApp->processEvents();

    int k1=0;
    int k2=bounds.nt();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            if( m_topHorizon ){
                auto z = m_topHorizon->valueAt(i,j);
                if( z==m_topHorizon->NULL_VALUE) continue;      // skip cdp without top horizon
                k1=std::max( 0, static_cast<int>( std::round( ( 0.001*z -bounds.ft() ) / bounds.dt() ) ) );        // maybe use this as time to sample in bounds
            }
            if( m_bottomHorizon ){
                auto z = m_bottomHorizon->valueAt(i,j);
                if( z==m_bottomHorizon->NULL_VALUE) continue;      // skip cdp without bottom horizon
                k2=std::min( bounds.nt(), static_cast<int>( std::round( ( 0.001*z -bounds.ft() ) / bounds.dt() ) ) );
            }
            for( int k=k1; k<k2; k++){

                auto ivalue1=(*m_inputVolume1)(i,j,k);
                m_processor.setInput1(ivalue1);

                if(m_inputVolume2){
                    auto ivalue2=(*m_inputVolume2)(i,j,k);
                    m_processor.setInput2(ivalue2);
                }

                (*m_outputVolume)(i,j,k)=m_processor.compute();
            }
        }

        emit progress( i-bounds.i1() );
        qApp->processEvents();

    }

    emit currentTask("Saving output volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( project()->existsVolume(m_outputName)){   // overwrite existing volume, must be input-volume 1 from dialog
        if( !project()->saveVolume( m_outputName, m_outputVolume )){    // XXX NEED TO handle process params!!!
            setErrorString( QString("Could not save volume \"%1\"!").arg(m_outputName) );
            return ResultCode::Error;
        }
    }
    else{       // new volume
        if( !project()->addVolume( m_outputName, m_outputVolume, params() )){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
            return ResultCode::Error;
        }
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
