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

    setParams(parameters);

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
    const Volume& vol=*m_inputVolume;

    std::shared_ptr<Volume> amplitudeVolume( new Volume(bounds) );
    std::shared_ptr<Volume> phaseVolume( new Volume(bounds) );
    std::shared_ptr<Volume> frequencyVolume( new Volume(bounds) );

    emit currentTask("Computing output volumes");
    emit started(bounds.ni());
    qApp->processEvents();

    auto null_value=vol.NULL_VALUE;

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            std::vector<float> trc(bounds.nt(),0);
            for( int k=0; k<bounds.nt(); k++){
                if( vol( i, j, k )!=vol.NULL_VALUE ) trc[k]= vol( i, j, k );
            }

            auto ctrc = hilbert_trace(trc);

            for( int k=0; k<bounds.nt(); k++){
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
            (*frequencyVolume)(i,j,bounds.nt()-1)=(abs(ctrc[bounds.nt()-1])-abs(ctrc[bounds.nt()-2]))/bounds.dt();
            for( int k=1; k+1<bounds.nt(); k++){
                (*frequencyVolume)(i,j,k)=(abs(ctrc[k+1])-abs(ctrc[k-1]))/(2*bounds.dt());
            }
        }

        emit progress(i-bounds.i1() );
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

            if( !project()->addVolume( p.first, p.second, params() ) ) {
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
