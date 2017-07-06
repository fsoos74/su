#include "instantaneousattributesprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<cmath>

#include "hilbert.h"

InstantaneousAttributesProcess::InstantaneousAttributesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Instantaneous Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode InstantaneousAttributesProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("input-volume"))){

        setErrorString("Parameters contain no input volume!");
        return ResultCode::Error;
    }
    QString inputName=parameters.value( QString("input-volume") );
    m_inputVolume=project()->loadVolume( inputName);
    if( !m_inputVolume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    if( !parameters.contains(QString("amplitude-volume"))){

        setErrorString("Parameters contain no amplitude volume!");
        return ResultCode::Error;
    }
    m_amplitudeVolumeName=parameters.value( QString("amplitude-volume") );

    if( !parameters.contains(QString("phase-volume"))){

        setErrorString("Parameters contain no phase volume!");
        return ResultCode::Error;
    }
    m_phaseVolumeName=parameters.value( QString("phase-volume") );


    if( !parameters.contains(QString("frequency-volume"))){

        setErrorString("Parameters contain no frequency volume!");
        return ResultCode::Error;
    }
    m_frequencyVolumeName=parameters.value( QString("frequency-volume") );

    return ResultCode::Ok;
}

ProjectProcess::ResultCode InstantaneousAttributesProcess::run(){

    Grid3DBounds bounds=m_inputVolume->bounds();
    const Grid3D<float>& vol=*m_inputVolume;

    std::shared_ptr<Grid3D<float>> amplitudeVolume( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> phaseVolume( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> frequencyVolume( new Grid3D<float>(bounds) );

    emit currentTask("Computing output volumes");
    emit started(bounds.inlineCount());
    qApp->processEvents();

    for( int i=bounds.inline1(); i<=bounds.inline2(); i++){

        for( int j=bounds.crossline1(); j<=bounds.crossline2(); j++){

            std::vector<float> trc(bounds.sampleCount());
            std::copy( &vol( i, j, 0 ), &vol( i, j, 0 ) + bounds.sampleCount(), trc.begin() );
            auto ctrc = hilbert_trace(trc);

            for( int k=0; k<bounds.sampleCount(); k++){
                (*amplitudeVolume)(i,j,k)=abs(ctrc[k]);
                (*phaseVolume)(i,j,k)=abs(ctrc[k]);
            }

            /*
            // first order approx.
            (*frequencyVolume)(i,j,0)=-(abs(ctrc[1])-abs(ctrc[0]))/bounds.dt();
            for( int k=1; k<bounds.sampleCount(); k++){
                (*frequencyVolume)(i,j,k)=(abs(ctrc[k])-abs(ctrc[k-1]))/bounds.dt();
            }
            */

            // second order approx.
            (*frequencyVolume)(i,j,0)=-(abs(ctrc[1])-abs(ctrc[0]))/bounds.dt();
            (*frequencyVolume)(i,j,bounds.sampleCount()-1)=(abs(ctrc[bounds.sampleCount()-1])-abs(ctrc[bounds.sampleCount()-2]))/bounds.dt();
            for( int k=1; k+1<bounds.sampleCount(); k++){
                (*frequencyVolume)(i,j,k)=(abs(ctrc[k+1])-abs(ctrc[k-1]))/(2*bounds.dt());
            }
        }

        emit progress(i-bounds.inline1() );
        qApp->processEvents();

    }

    emit currentTask("Saving result volumes");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    for( auto p : { std::make_pair(m_amplitudeVolumeName, amplitudeVolume),
                std::make_pair(m_phaseVolumeName, phaseVolume),
                std::make_pair(m_frequencyVolumeName, frequencyVolume)} ){

            if( p.first.isEmpty()) continue;

            if( !project()->addVolume( p.first, p.second ) ) {
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(p.first) );
            return ResultCode::Error;
        }
    }



    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
