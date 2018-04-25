#include "logintegrationprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include <QMap>

namespace{

QMap<LogIntegrationProcess::OP, QString> lookup{
    {LogIntegrationProcess::OP::Running_Sum, "Running Sum"},
    {LogIntegrationProcess::OP::Running_Average, "Running Average"},
    {LogIntegrationProcess::OP::Subtract_Previous, "Subtract Previous"}
};

}


QString LogIntegrationProcess::toQString(LogIntegrationProcess::OP op){
    return lookup.value(op, "Running Sum");
}

LogIntegrationProcess::OP LogIntegrationProcess::toOP(QString str){
    return lookup.key(str, LogIntegrationProcess::OP::Running_Sum);
}

QList<LogIntegrationProcess::OP> LogIntegrationProcess::ops(){
    return lookup.keys();
}

QStringList LogIntegrationProcess::opNames(){
    return lookup.values();
}


class RunningSum : public LogIntegrationProcess::IOP{
public:
    void start()override{
        m_sum=0;
    }
    double operator()(const double& x)override{
        if(x==nullValue()) return x;
        m_sum+=x;
        return m_sum;
    }
private:
    double m_sum=0;
};

class RunningAverage: public LogIntegrationProcess::IOP{
public:
    void start()override{
        m_sum=0;
        m_count=0;
    }
    double operator()(const double& x)override{
        if(x==nullValue()) return x;
        m_sum+=x;
        m_count++;
        return m_sum/m_count;   // m_count must be at least 1
    }
private:
    double m_sum=0;
    unsigned m_count=0;
};

class SubtractPrevious : public LogIntegrationProcess::IOP{
public:
    void start()override{
        m_prev=nullValue();
    }
    double operator()(const double& x)override{
        if( x==nullValue()){
            m_prev=x;
            return x;
        }
        if( m_prev==nullValue()){
            m_prev=x;
            return nullValue();
        }
        double res = x - m_prev;
        m_prev=x;
        return res;
    }
private:
    double m_prev=0;
};


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

        auto op=toOP(func);
        switch(op){
        case OP::Running_Sum:
            m_op=std::shared_ptr<IOP>(new RunningSum() ); break;
        case OP::Running_Average:
            m_op=std::shared_ptr<IOP>(new RunningAverage()); break;
        case OP::Subtract_Previous:
            m_op=std::shared_ptr<IOP>(new SubtractPrevious()); break;
        default:
            setErrorString("Invalid OP");
            return ResultCode::Error;
            break;
        }
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

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

    m_op->setNullValue(inputLog->NULL_VALUE);
    m_op->start();

    for( int i=0; i<outputLog->nz(); i++){

        auto ivalue=(*inputLog)[i];
        auto res=(*m_op)(ivalue);
        (*outputLog)[i]=res;
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
