#include "cropvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

CropVolumeProcess::CropVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Crop Volume"), project, parent){

}

ProjectProcess::ResultCode CropVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    int minMSec;
    int maxMSec;
    try{

        m_outputName=getParam(parameters, "output-volume");
        m_inputName=getParam(parameters, "input-volume");
        minIline=getParam(parameters, "min-iline").toInt();
        maxIline=getParam( parameters, "max-iline").toInt();
        minXline=getParam( parameters, "min-xline").toInt();
        maxXline=getParam( parameters, "max-xline").toInt();
        minMSec=getParam( parameters, "min-msec").toInt();
        maxMSec=getParam( parameters, "max-msec").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_inputVolume=project()->loadVolume(m_inputName);
    if( !m_inputVolume ){
        setErrorString(QString("Loading volume \"%1\" failed!").arg(m_inputName));
        return ResultCode::Error;
    }

    Grid3DBounds bounds=m_inputVolume->bounds();

    int il1=std::max(bounds.i1(), minIline);
    int il2=std::min(bounds.i2(), maxIline);
    if( il1>il2 ){
        setErrorString("Invalid output inline range!");
        return ResultCode::Error;
    }

    int xl1=std::max(bounds.j1(), minXline);
    int xl2=std::min(bounds.j2(), maxXline);
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

    m_volume=std::shared_ptr<Volume >( new Volume(outputBounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CropVolumeProcess::run(){


    Grid3DBounds bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            for( int k=0; k<bounds.nt(); k++){
                (*m_volume)(i,j,k)=m_inputVolume->value( i, j, bounds.sampleToTime(k));
            }
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
