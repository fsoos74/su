#include "creategridprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

CreateGridProcess::CreateGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Create Grid"), project, parent){

}


ProjectProcess::ResultCode CreateGridProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    float value;

    try{

        auto tname=getParam(parameters, "type");
        m_outputType=toGridType(tname);
        m_outputName=getParam(parameters, "name");
        minIline=getParam(parameters, "min-iline").toInt();
        maxIline=getParam( parameters, "max-iline").toInt();
        minXline=getParam( parameters, "min-xline").toInt();
        maxXline=getParam( parameters, "max-xline").toInt();
        value=getParam( parameters, "value").toFloat();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    Grid2DBounds bounds( minIline, minXline, maxIline, maxXline);
    m_grid=std::make_shared<Grid2D<float> >( bounds, value);

    if( !m_grid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CreateGridProcess::run(){

    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addGrid( m_outputType, m_outputName, m_grid, params() ) ){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
