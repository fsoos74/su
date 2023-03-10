#include "exportlasprocess.h"

#include <avoproject.h>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include<cmath>

ExportLASProcess::ExportLASProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Export LAS"), project, parent){

}

ProjectProcess::ResultCode ExportLASProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_wellName=getParam(parameters, "well");
        auto logs=getParam(parameters, "logs");
        m_logNames=unpackParamList(logs);
        m_outputPath=getParam(parameters, "output-file");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


QString buildMnemonic(QString name, QString unit, QString value, QString description){
    QString str=QString(" %1  .%2  %3  :%4").arg(name.leftJustified(8)).arg(unit.leftJustified(4)).arg(value,32).arg(description);
    return str;
}

ProjectProcess::ResultCode ExportLASProcess::run(){

    const QString NULL_VALUE("-999.25");
    const int WIDTH=12;

    QVector<std::shared_ptr<Log>> logs;
    for( auto name : m_logNames){
        auto log=project()->loadLog( m_wellName, name);
        if( !log){
            setErrorString(QString("Loading log \"%1-%2\" failed!").arg(m_wellName, name));
            return ResultCode::Error;
        }
        logs.push_back(log);
    }

    if(logs.isEmpty()){
        setErrorString("Need at least 1 log to export!");
        return ResultCode::Error;
    }

    for( int i=1; i<logs.size();i++){
        if( logs[i]->nz()!=logs[0]->nz()){
            setErrorString("Logs have different number of samples!");
            return ResultCode::Error;
        }
    }

    QFile file( m_outputPath );
    if ( !file.open(QIODevice::ReadWrite) ){
        setErrorString("Open output file failed!");
        return ResultCode::Error;
    }
    QTextStream os( &file );

    // write header
    //version
    os<<"~Version Information"<<endl;
    os<<buildMnemonic("VERS","i","2.0","CWLS log ASCII Standard - Version 2.0")<<endl;
    os<<buildMnemonic("WRAP","","NO","One Line per Depth Step")<<endl;
    //well
    os<<"~WELL Information Block"<<endl;
    os<<buildMnemonic("STRT","F",QString::number(logs[0]->z0()),"START DEPTH")<<endl;
    os<<buildMnemonic("STOP","F",QString::number(logs[0]->lz()),"STOP DEPTH")<<endl;
    os<<buildMnemonic("STEP","F",QString::number(logs[0]->dz()),"STEP")<<endl;
    os<<buildMnemonic("API","",m_wellName,"")<<endl;
    os<<buildMnemonic("UWI","",m_wellName,"")<<endl;
    os<<buildMnemonic("NULL","",NULL_VALUE,"NULL VALUE")<<endl;
    //curve info
    os<<"~Curve Information Block"<<endl;
    os<<buildMnemonic("DEPT","F","",":Depth autogenerated")<<endl;	// always add this column
    for(int i=0; i<logs.size(); i++){
        os<<buildMnemonic(logs[i]->name().c_str(),logs[i]->unit().c_str(),"",logs[i]->descr().c_str())<<endl;
    }
    os<<"~A"<<endl;
    emit started(logs[0]->nz());
    for( int i=0; i<logs[0]->nz(); i++){
        auto depthStr=QString("%1").arg(logs[0]->index2z(i),WIDTH);
        os<<depthStr<<"  ";
        for( int j=0; j<logs.size(); j++){
            auto value=logs[j]->at(i);
            QString valueStr;
            if(value!=logs[j]->NULL_VALUE && std::isfinite(value)){
                valueStr=QString("%1").arg(value, WIDTH);
            }
            else{
                valueStr=QString("%1").arg(NULL_VALUE,WIDTH);
            }
            os<<valueStr<<" ";
        }
        os<<endl;
        emit progress(i);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}
