#include "logmathprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include<functional>
using namespace std::placeholders;

LogMathProcess::LogMathProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Math"), project, parent){

}

ProjectProcess::ResultCode LogMathProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    double value=0;

    try{
        func=getParam(parameters, "function");

        m_outputName=getParam(parameters, "output-name");
        m_unit=getParam( parameters, "unit");
        m_descr=getParam(parameters, "description");

        m_inputName1=getParam(parameters, "input-log1");
        m_inputName2=getParam(parameters, "input-log2");

        value=getParam(parameters, "value").toDouble();

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


    m_processor.setOP(MathProcessor::toOP(func));
    m_processor.setValue(value);

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogMathProcess::processWell(QString well){

    emit currentTask(tr("Processing Well \"%1\"").arg(well));
    qApp->processEvents();

    auto inputLog1=project()->loadLog( well, m_inputName1);
    if( !inputLog1){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_inputName1));
        return ResultCode::Error;
    }

    std::shared_ptr<Log> inputLog2;
    if( !m_inputName2.isEmpty()){
        inputLog2=project()->loadLog( well, m_inputName2);
        if( !inputLog2){
            setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_inputName2));
            return ResultCode::Error;
        }
    }

    if( inputLog2){
        if( inputLog1->nz()!=inputLog2->nz()){
            setErrorString("Input logs contain different number of samples!");
            return ResultCode::Error;
        }
    }

    auto outputLog=std::make_shared<Log>( m_outputName.toStdString(), m_unit.toStdString(), m_descr.toStdString(),
                                       inputLog1->z0(), inputLog1->dz(), inputLog1->nz() );

    if( !outputLog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    m_processor.setInputNullValue(inputLog1->NULL_VALUE);

    for( int i=0; i<outputLog->nz(); i++){

        auto ivalue1=(*inputLog1)[i];
        m_processor.setInput1(ivalue1);

        if(inputLog2){
            auto ivalue2=(*inputLog2)[i];
            m_processor.setInput2(ivalue2);
        }

        auto res=m_processor.compute();

        if( res!=m_processor.NULL_VALUE) (*outputLog)[i]=res;
    }

    if( !project()->addLog( well, m_outputName, *outputLog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(well, m_outputName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogMathProcess::run(){

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
