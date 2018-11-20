#include "volumesimilarityprocess.h"

#include<memory>
#include<iostream>
#include "utilities.h"
#include <semblance.h>

#include<QApplication>

VolumeSimilarityProcess::VolumeSimilarityProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Similarity"), project, parent){

}

ProjectProcess::ResultCode VolumeSimilarityProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputname;
    QString ildipname;
    QString xldipname;

    try{
        m_volumeName=getParam(parameters, "output-volume");
        inputname=getParam( parameters, "input-volume");
        ildipname=getParam( parameters, "iline-dip");
        xldipname=getParam( parameters, "xline-dip");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_input=project()->loadVolume( inputname );
    if( !m_input ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    m_ildip=project()->loadVolume(ildipname);
    if( !m_ildip){
        setErrorString(tr("Loading volume \"%1\" failed!").arg(ildipname));
        return ResultCode::Error;
    }

    m_xldip=project()->loadVolume(xldipname);
    if( !m_xldip){
        setErrorString(tr("Loading volume \"%1\" failed!").arg(xldipname));
        return ResultCode::Error;
    }


    if( m_ildip->bounds()!=m_input->bounds()){
        setErrorString("Input and Dip volumes geometries don't match!");
        return ResultCode::Error;
    }
    if( m_ildip->bounds()!=m_xldip->bounds()){
        setErrorString("Dip volumes geometries don't match!");
        return ResultCode::Error;
    }

    auto bounds=m_input->bounds();
    m_volume=std::shared_ptr<Volume>(new Volume(bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode VolumeSimilarityProcess::run(){

    auto bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

#pragma omp parallel for
            for( int k=0; k<bounds.nt(); k++){
                (*m_volume)(i,j,k)=(*m_input)(i,j,k);
            }
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
            if(isCanceled()) return ResultCode::Canceled;
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addVolume( m_volumeName, m_volume, params() ) ) {
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    return ResultCode::Ok;
}


