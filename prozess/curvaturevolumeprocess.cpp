#include "curvaturevolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include<iostream>
#include<cmath>

CurvatureVolumeProcess::CurvatureVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Curvature Volume"), project, parent){

}

ProjectProcess::ResultCode CurvatureVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("output-volume"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output-volume"));


    if( !parameters.contains(QString("input-volume"))){

        setErrorString("Parameters contain no input volume!");
        return ResultCode::Error;
    }
    m_inputName=parameters.value( QString("input-volume") );


    m_inputVolume=project()->loadVolume( m_inputName);
    if( !m_inputVolume ){
        setErrorString("Loading input volume failed!");
        return ResultCode::Error;
    }


    m_volume=std::shared_ptr<Grid3D<float> >( new Grid3D<float>(m_inputVolume->bounds()));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CurvatureVolumeProcess::run(){


    Grid3DBounds bounds=m_volume->bounds();

    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1()+1; i<=bounds.inline2()-1; i++){

        for( int j=bounds.crossline1()+1; j<=bounds.crossline2()-1; j++){

            for( int k=1; k<bounds.sampleCount()-1; k++){

                float u=m_inputVolume->value( i, j, bounds.sampleToTime(k));
                if( u==m_inputVolume->NULL_VALUE ) continue;

                float ui1=m_inputVolume->value( i-1, j, bounds.sampleToTime(k));
                float ui2=m_inputVolume->value( i+1, j, bounds.sampleToTime(k));
                if( ui1==m_inputVolume->NULL_VALUE || ui2==m_inputVolume->NULL_VALUE) continue;
                float vi = ( ui1 + ui2 - 2*u );

                float uj1=m_inputVolume->value( i, j-1, bounds.sampleToTime(k));
                float uj2=m_inputVolume->value( i, j+1, bounds.sampleToTime(k));
                if( uj1==m_inputVolume->NULL_VALUE || uj2==m_inputVolume->NULL_VALUE) continue;
                float vj = ( uj1 + uj2 - 2*u );

                float uk1=m_inputVolume->value( i, j, bounds.sampleToTime(k-1));
                float uk2=m_inputVolume->value( i, j, bounds.sampleToTime(k+1));
                if( uk1==m_inputVolume->NULL_VALUE || uk2==m_inputVolume->NULL_VALUE) continue;
                float vk = ( uk1 + uk2 - 2*u );


                float v = std::sqrt( vi*vi + vj*vj + vk*vk );

                (*m_volume)(i,j,k)=v;
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

    if( !project()->addVolume( m_outputName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
