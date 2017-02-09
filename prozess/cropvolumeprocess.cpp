#include "cropvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include<iostream>

CropVolumeProcess::CropVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Secondary Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode CropVolumeProcess::init( const QMap<QString, QString>& parameters ){

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


    if( !parameters.contains(QString("min-iline"))){

        setErrorString("Parameters contain no minimum inline!");
        return ResultCode::Error;
    }
    int minIline=parameters.value( QString("min-iline") ).toInt();

    if( !parameters.contains(QString("max-iline"))){

        setErrorString("Parameters contain no maximum inline!");
        return ResultCode::Error;
    }
    int maxIline=parameters.value( QString("max-iline") ).toInt();

    if( !parameters.contains(QString("min-xline"))){

        setErrorString("Parameters contain no minimum crossline!");
        return ResultCode::Error;
    }
    int minXline=parameters.value( QString("min-xline") ).toInt();

    if( !parameters.contains(QString("max-xline"))){

        setErrorString("Parameters contain no maximum crossline!");
        return ResultCode::Error;
    }
    int maxXline=parameters.value( QString("max-xline") ).toInt();


    if( !parameters.contains(QString("min-msec"))){

        setErrorString("Parameters contain no minimum time!");
        return ResultCode::Error;
    }
    int minMSec=parameters.value( QString("min-msec") ).toInt();

    if( !parameters.contains(QString("max-msec"))){

        setErrorString("Parameters contain no maximum time!");
        return ResultCode::Error;
    }
    int maxMSec=parameters.value( QString("max-msec") ).toInt();


    m_inputVolume=project()->loadVolume( m_inputName);
    if( !m_inputVolume ){
        setErrorString("Loading input volume failed!");
        return ResultCode::Error;
    }



    Grid3DBounds bounds=m_inputVolume->bounds();

    int il1=std::max(bounds.inline1(), minIline);
    int il2=std::min(bounds.inline2(), maxIline);
    if( il1>il2 ){
        setErrorString("Invalid output inline range!");
        return ResultCode::Error;
    }

    int xl1=std::max(bounds.crossline1(), minXline);
    int xl2=std::min(bounds.crossline2(), maxXline);
    if( xl1>xl2 ){
        setErrorString("Invalid output crossline range!");
        return ResultCode::Error;
    }

    double ft=std::max( 0.001*minMSec, bounds.ft() );
    double lt=std::min( 0.001*maxMSec, bounds.lt() );
    double dt=bounds.dt();
    if( !dt ){
        setErrorString("Input volume has invalid sampling interval!");
        return ResultCode::Error;
    }
    if( ft>lt ){
        setErrorString("Invalid output time range!");
        return ResultCode::Error;
    }

    int nt=1 + ( lt - ft )/dt;

/*
    std::cout<<"il1="<<il1<<" il2="<<il2<<std::endl;
    std::cout<<"xl1="<<xl1<<" xl2="<<xl2<<std::endl;
    std::cout<<"nt="<<nt<<" dt="<<dt<<" ft="<<ft<<std::endl;
*/
    Grid3DBounds outputBounds( il1, il2, xl1, xl2, nt, ft, dt );

    m_volume=std::shared_ptr<Grid3D<float> >( new Grid3D<float>(outputBounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CropVolumeProcess::run(){


    Grid3DBounds bounds=m_volume->bounds();

    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1(); i<=bounds.inline2(); i++){

        for( int j=bounds.crossline1(); j<=bounds.crossline2(); j++){

            for( int k=0; k<bounds.sampleCount(); k++){
                (*m_volume)(i,j,k)=m_inputVolume->value( i, j, bounds.sampleToTime(k));
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
