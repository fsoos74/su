#include "cropvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

CropVolumeProcess::CropVolumeProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Crop Volume"), project, parent){

}

ProjectProcess::ResultCode CropVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputName;
    QString outputName;
    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    int minMSec;
    int maxMSec;
    try{

        outputName=getParam(parameters, "output-volume");
        inputName=getParam(parameters, "input-volume");
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

    auto res=addInputVolume(inputName);
    if( res!=ResultCode::Ok) return res;

    Grid3DBounds ibounds=inputBounds(0);

    int il1=std::max(ibounds.i1(), minIline);
    int il2=std::min(ibounds.i2(), maxIline);
    if( il1>il2 ){
        setErrorString("Invalid output inline range!");
        return ResultCode::Error;
    }

    int xl1=std::max(ibounds.j1(), minXline);
    int xl2=std::min(ibounds.j2(), maxXline);
    if( xl1>xl2 ){
        setErrorString("Invalid output crossline range!");
        return ResultCode::Error;
    }

    double ft=std::max( 0.001*minMSec, ibounds.ft() );
    double lt=std::min( 0.001*maxMSec, ibounds.lt() );
    if( ft>lt ){
        setErrorString("Invalid output time range!");
        return ResultCode::Error;
    }

    double dt=ibounds.dt();
    if( dt<=0 ){
        setErrorString("Input volume has invalid sampling interval!");
        return ResultCode::Error;
    }


    int nt=1 + ( lt - ft )/dt;

    auto bounds=Grid3DBounds( il1, il2, xl1, xl2, nt, ft, dt );
    setBounds(bounds);

    res=addOutputVolume(outputName, bounds, inputDomain(0), inputType(0) );
    if( res!=ResultCode::Ok) return res;

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CropVolumeProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    auto inputVolume=inputs[0];
    auto outputVolume=outputs[0];

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        for( int k=0; k<bounds().nt(); k++){
            auto t=bounds().sampleToTime(k);
            (*outputVolume)(iline,j,k)=inputVolume->value( iline, j, t);     // also works if new ft is not on a sample
        }
    }

    return ResultCode::Ok;
}
