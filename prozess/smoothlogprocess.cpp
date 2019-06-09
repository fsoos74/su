#include "smoothlogprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>


SmoothLogProcess::SmoothLogProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Smooth Log"), project, parent){

}

ProjectProcess::ResultCode SmoothLogProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    QString iname;
    QString unit;
    QString descr;
    int aperture;

    try{
        func=getParam(parameters, "method");
        m_aperture=getParam( parameters, "aperture").toInt();
        m_retainValues=static_cast<bool>( getParam( parameters, "retain-values").toInt());
        m_outputName=getParam(parameters, "output-log");
        m_unit=getParam( parameters, "unit");
        m_descr=getParam(parameters, "description");

        m_inputName=getParam(parameters, "input-log");

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

    m_processor.setOP(SmoothProcessor::toOP(func));

    return ResultCode::Ok;
}


ProjectProcess::ResultCode SmoothLogProcess::processWell(QString well){

    auto inputLog=project()->loadLog(well, m_inputName);
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

    int hw=m_aperture/2;

    m_processor.setInputNullValue(inputLog->NULL_VALUE);

    for( int i=0; i<outputLog->nz(); i++){
        double res=inputLog->at(i);

        // compute new value only if required
        if( !m_retainValues || res==inputLog->NULL_VALUE){
            m_processor.clearInput();
            int jmin=std::max( 0, i-hw);
            int jmax=std::min( outputLog->nz()-1, i+hw);
            for( int j=jmin; j<=jmax; j++){
                auto value=(*inputLog)[j];
                auto dist=std::abs(i-j);
                m_processor.addInput(value, dist);
            }
            res=m_processor.compute();
        }

        (*outputLog)[i]=res;
    }

    if( !project()->addLog( well, m_outputName, *outputLog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(well, m_outputName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode SmoothLogProcess::run(){

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
