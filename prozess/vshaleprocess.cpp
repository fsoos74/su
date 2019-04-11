#include "vshaleprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include<smoothprocessor.h>
#include<topsdbmanager.h>
#include<QMap>

namespace{
QMap<VShaleProcess::BlockingMode, QString> lookup{ {VShaleProcess::BlockingMode::Curves, "Curves"},
                                                 {VShaleProcess::BlockingMode::LayersTops, "Layers-Tops"}};
}

QString toQString(VShaleProcess::BlockingMode m){
    return lookup.value(m, "Curves");
}

VShaleProcess::BlockingMode toBlockingMode(QString s){
    return lookup.key(s, VShaleProcess::BlockingMode::Curves);
}


VShaleProcess::VShaleProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("VShale Computation"), project, parent){

}

ProjectProcess::ResultCode VShaleProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_wells=unpackParamList( getParam(parameters, "wells") );
        m_grName=getParam(parameters, "gr-log");
        m_igrName=getParam(parameters, "igr-log");
        m_vshTertiaryRocksName=getParam(parameters, "vsh-tertiary-rocks-log");
        m_vshOlderRocksName=getParam(parameters, "vsh-older-rocks-log");
        m_vshSteiberName=getParam(parameters, "vsh-steiber-log");
        m_vshClavierName=getParam(parameters, "vsh-clavier-log");
        m_grMinName=getParam(parameters, "grmin-log");
        m_grMaxName=getParam(parameters, "grmax-log");
        auto s=getParam(parameters, "blocking-mode");
        m_blockingMode=toBlockingMode(s);
        percentage=getParam(parameters,"blocking-percentage").toInt();
        s=getParam(parameters, "blocking-tops");
        m_tops=s.split(";", QString::SkipEmptyParts);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


void processLog( Log& outputLog, const Log& inputLog, int aperture, SmoothProcessor::OP op){

    SmoothProcessor processor;
    processor.setOP(op);
    processor.setInputNullValue(inputLog.NULL_VALUE);
    int hw=aperture/2;

    for( int i=0; i<outputLog.nz(); i++){

        processor.clearInput();

        int jmin=std::max( 0, i-hw);
        int jmax=std::min( outputLog.nz()-1, i+hw);
        for( int j=jmin; j<=jmax; j++){
            auto value=inputLog[j];
            auto dist=std::abs(i-j);
            processor.addInput(value, dist);
        }

        auto res=processor.compute();

        if( res!=processor.NULL_VALUE) outputLog[i]=res;
    }

}


void VShaleProcess::blockLogMin( Log& outputLog, const Log& inputLog, int i0, int i1){
    auto min=std::numeric_limits<double>::max();
    for( int i=i0; i<i1; i++){
        if( inputLog[i]==inputLog.NULL_VALUE) continue;
        if( inputLog[i]<min ) min=inputLog[i];
    }
    for( int i=i0; i<i1; i++){
        outputLog[i]=min;
    }
}

void VShaleProcess::blockLogMax( Log& outputLog, const Log& inputLog, int i0, int i1){

    auto max=std::numeric_limits<double>::lowest();
    for( int i=i0; i<i1; i++){
        if( inputLog[i]==inputLog.NULL_VALUE) continue;
        if( inputLog[i]>max ) max=inputLog[i];
    }
    for( int i=i0; i<i1; i++){
        outputLog[i]=max;
    }
}

void VShaleProcess::blockLogMinMax( Log& minLog, Log& maxLog, const Log& inputLog, int i0, int i1){
    std::vector<double> buf;
    int n=i1-i0;
    if(n<=0) return;
    buf.reserve(n);
    for(int i=i0; i<=i1; i++){
        buf.push_back(inputLog[i]);
    }
    std::sort(buf.begin(), buf.end());

    double keep=0.01*percentage;
    int min=0.5*(1.0-keep)*buf.size();
    int max=buf.size()-min;  // symmetric
    for(int i=i0; i<i1; i++){
        minLog[i]=buf[min];
        maxLog[i]=buf[max];
    }
}

void blockLogMax( Log& outputLog, const Log& inputLog, int aperture){

    for( int i0=0; i0<outputLog.nz(); i0+=aperture){

        int i1=std::min(i0+aperture, outputLog.nz());
        auto max=std::numeric_limits<double>::lowest();
        for( int i=i0; i<i1; i++){
            if( inputLog[i]==inputLog.NULL_VALUE) continue;
            if( inputLog[i]>max ) max=inputLog[i];
        }
        for( int i=i0; i<i1; i++){
            outputLog[i]=max;
        }
    }
}

#include<iostream>
ProjectProcess::ResultCode VShaleProcess::run(){

    emit started(m_wells.size());
    qApp->processEvents();

    int i=0;
    for( auto well : m_wells){

        auto res=processWell(well);
        if( res!=ResultCode::Ok) return res;

        emit progress(++i);
        qApp->processEvents();

        if( isCanceled()) return ResultCode::Canceled;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode VShaleProcess::processWell(QString well){
    auto grLog=project()->loadLog( well, m_grName);
    if( !grLog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_grName));
        return ResultCode::Error;
    }

    std::cout<<well.toStdString()<<" "<<grLog->name()<<" "<<grLog->nz()<<std::endl<<std::flush;

    std::shared_ptr<Log> grMinLog;
    std::shared_ptr<Log> grMaxLog;
    if(m_blockingMode==BlockingMode::Curves){
        grMinLog=project()->loadLog(well, m_grMinName);
        if( !grMinLog){
            setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_grMinName));
            return ResultCode::Error;
        }
        grMaxLog=project()->loadLog(well, m_grMaxName);
        if( !grMaxLog){
            setErrorString(QString("Loading log \"%1-%2\" failed!").arg(well, m_grMaxName));
            return ResultCode::Error;
        }
    }
    else{
        grMinLog=std::make_shared<Log>( m_grMinName.toStdString(), grLog->unit(), "gamma ray min envelope",
                                           grLog->z0(), grLog->dz(), grLog->nz() );

        grMaxLog=std::make_shared<Log>( m_grMaxName.toStdString(), grLog->unit(), "gamma ray max envelope",
                                           grLog->z0(), grLog->dz(), grLog->nz() );
    }

    auto igrLog=std::make_shared<Log>( m_igrName.toStdString(), "fraction", "gamma ray index",
                                       grLog->z0(), grLog->dz(), grLog->nz() );

    auto vshTertiaryRocksLog=std::make_shared<Log>( m_vshTertiaryRocksName.toStdString(), "V/V", "VSHALE for tertiary rocks (Larionov)",
                                       grLog->z0(), grLog->dz(), grLog->nz() );

    auto vshOlderRocksLog=std::make_shared<Log>( m_vshOlderRocksName.toStdString(), "V/V", "VSHALE for older rocks (Larionov)",
                                       grLog->z0(), grLog->dz(), grLog->nz() );

    auto vshSteiberLog=std::make_shared<Log>( m_vshSteiberName.toStdString(), "V/V", "VSHALE (Steiber)",
                                       grLog->z0(), grLog->dz(), grLog->nz() );

    auto vshClavierLog=std::make_shared<Log>( m_vshClavierName.toStdString(), "V/V", "VSHALE (Clavier)",
                                       grLog->z0(), grLog->dz(), grLog->nz() );

    if( !igrLog || !vshTertiaryRocksLog || !vshOlderRocksLog || !vshSteiberLog || !vshClavierLog || !grMinLog || !grMaxLog ){
        std::cout<<"!!!1E"<<std::endl<<std::flush;
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    // compute envelopes/blocking
    if(m_blockingMode==BlockingMode::LayersTops){
        auto topsDB=project()->openTopsDatabase();
        if(!topsDB){
            setErrorString("Loading tops database failed!");
            return ResultCode::Error;
        }
        QVector<double> depths;
        for(auto name : m_tops){
            if( !topsDB->exists(well, name)){
                //setErrorString(tr("Well %1 has no top%2").arg(well,name));
                //return ResultCode::Error;
                continue;	// silently ignore for now - need to issue warning
            }
            auto depth = topsDB->value(well, name);
            depths<<depth;
        }
        // add start/end
        depths<<grLog->z0();
        depths<<grLog->lz();

        std::sort(depths.begin(), depths.end());
        for( int i = 1; i<depths.size(); i++){
            auto topz=depths[i-1];
            auto botz=depths[i];
            auto i0=grLog->z2index(topz);
            auto i1=grLog->z2index(botz);
            i0=std::max(0,i0);
            i1=std::min(i1,grLog->nz());
            //blockLogMin(*grMinLog, *grLog, i0, i1);
            //blockLogMax(*grMaxLog, *grLog, i0, i1);
            blockLogMinMax(*grMinLog,*grMaxLog, *grLog, i0, i1);
        }
        // store blocked min/max logs
        QString minName=QString("%1_block_min%2").arg(grLog->name().c_str()).arg(percentage);
        grMinLog->setName(minName.toStdString());
        if(!project()->saveLog(well, minName, *grMinLog)){
            setErrorString("Saving blocked minimum gr log failed!");
            return ResultCode::Error;
        }
        QString maxName=QString("%1_block_max%2").arg(grLog->name().c_str()).arg(percentage);
        grMaxLog->setName(maxName.toStdString());
        if(!project()->saveLog(well, maxName, *grMaxLog)){
            setErrorString("Saving blocked maximum gr log failed!");
            return ResultCode::Error;
        }
    }
    // finally compute igr, vshale
    for( int i=0; i<grLog->nz(); i++){

        auto gr = (*grLog)[i];
        auto grMin = (*grMinLog)[i];
        auto grMax = (*grMaxLog)[i];
        if( grMin==grMinLog->NULL_VALUE || grMax==grMaxLog->NULL_VALUE || grMin==grMax ) continue;
        auto igr = (gr-grMin)/(grMax-grMin);
        // clip to [0,1]
        if( igr<0 ) igr=0;
        if( igr>1 ) igr=1;
        auto vshTertiaryRocks = 0.083 * ( std::pow( 2, 3.7*igr ) - 1 );
        auto vshOlderRocks = 0.33 * ( std::pow( 2, 2*igr) - 1 );
        auto vshSteiber = igr / ( 3 - 2*igr);
        auto vshClavier = 1.7 - std::sqrt( 3.38 - std::pow( igr+0.7, 2 ) );

       (*igrLog)[i]=igr;
       (*vshTertiaryRocksLog)[i]=vshTertiaryRocks;
       (*vshOlderRocksLog)[i]=vshOlderRocks;
       (*vshSteiberLog)[i]=vshSteiber;
       (*vshClavierLog)[i]=vshClavier;
    }

    for( std::pair<QString, std::shared_ptr<Log>> namelog : {
                          std::make_pair( m_igrName, igrLog),
                          std::make_pair( m_vshTertiaryRocksName, vshTertiaryRocksLog),
                          std::make_pair( m_vshOlderRocksName, vshOlderRocksLog),
                          std::make_pair( m_vshSteiberName, vshSteiberLog),
                          std::make_pair( m_vshClavierName, vshClavierLog)} ){
        if( !namelog.first.isEmpty() && !project()->addLog( well, namelog.first, *namelog.second ) ){
            setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(well, namelog.first) );
            return ResultCode::Error;
        }
    }

    return ResultCode::Ok;
}
