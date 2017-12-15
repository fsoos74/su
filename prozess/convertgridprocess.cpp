#include "convertgridprocess.h"

#include <avoproject.h>

ConvertGridProcess::ConvertGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Convert Grid"), project, parent){

}

ProjectProcess::ResultCode ConvertGridProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    if( !parameters.contains(QString("output-type"))){
        setErrorString("Parameters contain no output grid type!");
        return ResultCode::Error;
    }
    m_outputType = toGridType(parameters.value(QString("output-type") ) );

    if( !parameters.contains(QString("output-name"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output-name"));

    if( !parameters.contains(QString("input-type"))){
        setErrorString("Parameters contain no input grid type!");
        return ResultCode::Error;
    }
    m_inputType = toGridType(parameters.value(QString("input-type") ) );

    if( !parameters.contains(QString("input-name"))){
        setErrorString("Parameters contain no input grid!");
        return ResultCode::Error;
    }
    m_inputName=parameters.value(QString("input-name"));

    //load input grid
    m_inputGrid=project()->loadGrid( m_inputType, m_inputName);
    if( !m_inputGrid ){
        setErrorString("Loading grid failed!");
        return ResultCode::Error;
    }

    Grid2DBounds bounds=m_inputGrid->bounds();
    m_outputGrid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));

    if( !m_outputGrid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode ConvertGridProcess::run(){

    Grid2DBounds bounds=m_inputGrid->bounds();

    emit currentTask("Iterating cdps");
    emit started(bounds.height());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

           (*m_outputGrid)(i,j)=(*m_inputGrid)(i,j);

        }

        emit progress(i-bounds.i1());
        qApp->processEvents();
    }


    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addGrid( m_outputType, m_outputName, m_outputGrid, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


