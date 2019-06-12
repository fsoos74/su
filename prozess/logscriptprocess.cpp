


#include "logscriptprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>
#include <wellpath.h>
#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>
#include<QScriptEngine>
#include<QScriptValue>
#include<QScriptValueList>

LogScriptProcess::LogScriptProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Script"), project, parent){

}

ProjectProcess::ResultCode LogScriptProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);



    try{

        m_outputName=getParam(parameters, "log");
        m_unit=getParam(parameters, "unit");
        m_descr=getParam(parameters, "description");
        m_script=getParam(parameters, "script");

        auto ninputs=getParam(parameters, "n_input_logs").toInt();
        for( int i=0; i<ninputs; i++){
            QString key=QString("input_log_%1").arg(i+1);
            auto name=getParam(parameters, key);
            m_inputNames.push_back(name);
        }

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

    return ResultCode::Ok;
}

ProjectProcess::ResultCode LogScriptProcess::initWell(QString well){

    //load input logs
    m_inputLogs.clear();
    for( auto lname : m_inputNames){
        auto log=project()->loadLog(well, lname);
        if( !log ){
            setErrorString(QString("Loading log \"%1 - %2\" failed!").arg(well, lname));
            return ResultCode::Error;
        }
        m_inputLogs.push_back(log);
    }

    if( m_inputLogs.size()<1){
        setErrorString("Need at least 1 input log");
        return ResultCode::Error;
    }

    // test if all input logs have same number of samples
    for( int i=1; i<m_inputLogs.size(); i++){
        if( m_inputLogs[0]->nz() != m_inputLogs[i]->nz()){
            setErrorString("Not all logs have the same number of samples");
            return ResultCode::Error;
        }
    }

    // allocate outut log
    m_log=std::make_shared<Log>(m_outputName.toStdString(), m_unit.toStdString(), m_descr.toStdString(),
                                m_inputLogs[0]->z0(), m_inputLogs[0]->dz(), m_inputLogs[0]->nz());
    if( !m_log ){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode LogScriptProcess::processWell(QString well){

   QScriptEngine engine;
   QScriptValue fun=engine.evaluate(tr("(") +m_script+ tr(")"));
   if(engine.hasUncaughtException()){
       int line = engine.uncaughtExceptionLineNumber();
       auto message=engine.uncaughtException().toString();
       setErrorString(QString("Error in line %1: %2").arg(QString::number(line), message));
       return ResultCode::Error;
   }

   for( int i=0; i<m_log->nz(); i++){
      QScriptValueList args;
      for( int k=0; k<m_inputLogs.size(); k++){
           args<<(*m_inputLogs[k])[i];
      }
      QScriptValue result = fun.call(QScriptValue(), args);
      if(engine.hasUncaughtException()){
          setErrorString(tr("Error occured: ")+result.toString());
          return ResultCode::Error;
      }
      auto res=result.toNumber();
      (*m_log)[i]=std::isfinite(res) ? res : m_log->NULL_VALUE;
   }


    if( !project()->addLog( well, m_log->name().c_str(), *m_log) ){
        setErrorString( QString("Could not add log \"%1\" to project!").arg(m_log->name().c_str()) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogScriptProcess::run(){

    try{

    auto res=ResultCode::Ok;

    emit started(m_wells.size());
    int n=0;

    for( auto well : m_wells){

        initWell(well);

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


    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
