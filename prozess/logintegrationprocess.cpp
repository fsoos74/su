#include "logintegrationprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include<functional>
using namespace std::placeholders;

LogIntegrationProcess::LogIntegrationProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Integration"), project, parent){

}

ProjectProcess::ResultCode LogIntegrationProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;

    try{
        func=getParam(parameters, "function");

        m_outputName=getParam(parameters, "output-name");
        m_unit=getParam( parameters, "unit");
        m_descr=getParam(parameters, "description");

        m_inputName=getParam(parameters, "input-log");

        m_startValue=getParam(parameters, "start-value").toDouble();

        int i=0;
        while(1){
           QString name=QString("well-%1").arg(++i);
           if( parameters.contains(name)){
               auto well=parameters.value(name);
               m_wells.push_back(well);
           }
           else{
               break;
           }
        }
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }


    m_processor.setOP(IntegrationProcessor::toOP(func));

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogIntegrationProcess::processWell(QString well){

    auto inputLog=project()->loadLog( well, m_inputName);
    if( !inputLog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_inputName));
        return ResultCode::Error;
    }

    auto outputLog=std::make_shared<Log>( m_outputName.toStdString(), m_unit.toStdString(), m_descr.toStdString(),
                                       inputLog->z0(), inputLog->dz(), inputLog->nz() );

    if( !outputLog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    m_processor.setInputNullValue(inputLog->NULL_VALUE);
    m_processor.restart(m_startValue);

    for( int i=0; i<outputLog->nz(); i++){

        auto ivalue=(*inputLog)[i];
        m_processor.addValue(ivalue);
        auto res=m_processor.compute();
        if( res!=m_processor.NULL_VALUE) (*outputLog)[i]=res;
    }

    if( !project()->addLog( well, m_outputName, *outputLog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(well, m_outputName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogIntegrationProcess::run(){

    emit started(m_wells.size());
    int n=0;

    for( auto well : m_wells){

        emit currentTask(tr("Processing Well \"%1\"").arg(well));
        qApp->processEvents();

        auto res=processWell(well);
        if( res!=ResultCode::Ok) return res;

        n++;
        emit progress(n);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}
