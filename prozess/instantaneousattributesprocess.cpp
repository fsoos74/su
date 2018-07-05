#include "instantaneousattributesprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<cmath>

#include "hilbert.h"

InstantaneousAttributesProcess::InstantaneousAttributesProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Instantaneous Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode InstantaneousAttributesProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputVolumeName;
    QString amplitudeVolumeName, frequencyVolumeName, phaseVolumeName;

    try{
        inputVolumeName=getParam(parameters, "input-volume");
        amplitudeVolumeName=getParam(parameters, "amplitude-volume");
        frequencyVolumeName=getParam(parameters, "frequency-volume");
        phaseVolumeName=getParam(parameters, "phase-volume");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    auto res=addInputVolume(inputVolumeName);
    if(res!=ResultCode::Ok) return res;

    setBounds(inputBounds(0));  // bounds of input volume determine output volumes bounds
    for( auto name : {amplitudeVolumeName, frequencyVolumeName, phaseVolumeName}){      // order matters!
        auto res=addOutputVolume(name, bounds(), inputDomain(0), VolumeType::Other);
        if(res!=ResultCode::Ok) return res;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode InstantaneousAttributesProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    std::shared_ptr<Volume> inputVolume=inputs[0];
    std::shared_ptr<Volume> amplitudeVolume=outputs[0];
    std::shared_ptr<Volume> frequencyVolume=outputs[1];
    std::shared_ptr<Volume> phaseVolume=outputs[2];

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        std::vector<float> trc(bounds().nt(),0);
        for( int k=0; k<bounds().nt(); k++){
            auto val=(*inputVolume)(iline,j,k);
            if(val!=inputVolume->NULL_VALUE) trc[k]=val;
        }

        auto ctrc = hilbert_trace(trc);

        for( int k=0; k<bounds().nt(); k++){
            (*amplitudeVolume)(iline,j,k)=abs(ctrc[k]);
            (*phaseVolume)(iline,j,k)=abs(ctrc[k]);
        }

        /*
        // first order approx.
        (*frequencyVolume)(i,j,0)=-(abs(ctrc[1])-abs(ctrc[0]))/bounds.dt();
        for( int k=1; k<bounds.sampleCount(); k++){
            (*frequencyVolume)(i,j,k)=(abs(ctrc[k])-abs(ctrc[k-1]))/bounds.dt();
        }
        */

        // second order approx.
        (*frequencyVolume)(iline,j,0)=-(abs(ctrc[1])-abs(ctrc[0]))/bounds().dt();
        (*frequencyVolume)(iline,j,bounds().nt()-1)=(abs(ctrc[bounds().nt()-1])-abs(ctrc[bounds().nt()-2]))/bounds().dt();
        for( int k=1; k+1<bounds().nt(); k++){
            (*frequencyVolume)(iline,j,k)=(abs(ctrc[k+1])-abs(ctrc[k-1]))/(2*bounds().dt());
        }
    }

    return ResultCode::Ok;
}
