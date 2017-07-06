#include "semblancevolumeprocess.h"

#include<grid2d.h>
#include<memory>
#include<iostream>



#include "utilities.h"
#include <semblance.h>

#include<QApplication>

SemblanceVolumeProcess::SemblanceVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Semblance Volume"), project, parent){

}

ProjectProcess::ResultCode SemblanceVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("half-ilines"))){
        setErrorString("Parameters contain no half-ilines");
        return ResultCode::Error;
    }
    m_halfIlines=parameters.value(QString("half-ilines") ).toUInt();

    if( !parameters.contains(QString("half-xlines"))){
        setErrorString("Parameters contain no half-xlines");
        return ResultCode::Error;
    }
    m_halfXlines=parameters.value(QString("half-xlines") ).toUInt();

    if( !parameters.contains(QString("half-samples"))){
        setErrorString("Parameters contain no half-samples");
        return ResultCode::Error;
    }
    m_halfSamples=parameters.value(QString("half-samples") ).toUInt();

    if( !parameters.contains(QString("input-volume"))){

        setErrorString("Parameters contain no input-volume!");
        return ResultCode::Error;
    }
    QString inputName=parameters.value( QString("input-volume") );
    m_inputVolume=project()->loadVolume( inputName );
    if( !m_inputVolume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }
    m_bounds=m_inputVolume->bounds();

    if( !parameters.contains(QString("output-volume"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("output-volume"));

    m_volume=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode SemblanceVolumeProcess::run(){

    auto bounds=m_volume->bounds();

    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1()+1; i<=bounds.inline2()-1; i++){

        for( int j=bounds.crossline1()+1; j<=bounds.crossline2()-1; j++){

            std::vector<std::pair<int,int>> locations;
            for( int ii=i-m_halfIlines; ii<=i+m_halfIlines; ii++){
                for( int jj=j-m_halfXlines; jj<=j+m_halfXlines; jj++){
                    if( bounds.isInside(ii,jj)){
                        locations.push_back( std::make_pair(ii,jj) );
                    }
                }
            }

            #pragma omp parallel for
            for( int k=0; k<bounds.sampleCount(); k++){
                (*m_volume)(i,j,k)=semblance(*m_inputVolume, locations, m_halfSamples, k);
            }

        }

        if( (i-bounds.inline1()) % onePercent==0 ){
            emit progress((i-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addVolume( m_volumeName, m_volume ) ) {
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    return ResultCode::Ok;
}


