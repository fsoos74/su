#include "volumetdconversionprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<QApplication>
#include<omp.h>

VolumeTDConversionProcess::VolumeTDConversionProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Volume TD Conversion"), project, parent){

}

ProjectProcess::ResultCode VolumeTDConversionProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    QString oname;
    QString iname;
    QString vname;

    try{
        oname=getParam(parameters, "output-volume");
        iname=getParam(parameters, "input-volume");
        vname=getParam(parameters, "velocity-volume");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    ProjectProcess::ResultCode res=addInputVolume(iname);
    if(res!=ResultCode::Ok) return res;
    res=addInputVolume(vname);
    if(res!=ResultCode::Ok) return res;

    auto ibounds=inputBounds(0);                // input volume
    auto vbounds=inputBounds(1);                // depth velocity volume
    auto i1=std::max(ibounds.i1(),vbounds.i1());
    auto i2=std::min(ibounds.i2(),vbounds.i2());
    auto j1=std::max(ibounds.j1(),vbounds.j1());
    auto j2=std::min(ibounds.j2(),vbounds.j2());
    // output volume sampling from velocity volume, assume vels start at 0!!!
    auto z0=vbounds.ft();
    auto dz=vbounds.dt();
    auto nz=vbounds.nt();
    auto obounds=Grid3DBounds(i1,i2, j1,j2, nz,z0,dz );
    setBounds(obounds);

    res=addOutputVolume(oname, bounds(), Domain::Depth, inputType(0));
    if( res!=ResultCode::Ok) return res;

    return ResultCode::Ok;
}

ProjectProcess::ResultCode VolumeTDConversionProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){
    std::shared_ptr<Volume> input=inputs[0];
    std::shared_ptr<Volume> ivel=inputs[1];
    std::shared_ptr<Volume> output=outputs[0];

    //compute average velocity in depth
    auto avel=std::make_shared<Volume>(ivel->bounds(), Domain::Depth, VolumeType::AVEL, 0);
#pragma omp parallel for
    for( int j=bounds().j1(); j<=bounds().j2(); j++){
        double sum=0;
        for( int k=0; k<bounds().nt(); k++){
             sum+=(*ivel)(iline,j,k);
             (*avel)(iline,j,k)=sum/(k+1);
        }
    }

    // finally convert samples
#pragma omp parallel for
    for( int j=bounds().j1(); j<=bounds().j2(); j++){
        for( int k=0; k<bounds().nt(); k++){
            auto z=bounds().sampleToTime(k);		// Time in sampling is depth for depth data!!!
            auto v=(*avel)(iline,j,k);
            auto t=2*1000*z/v;						// TIME IS TWO WAY TIME
            if( k==500) std::cout<<z<<" "<<v<<" "<<t<<std::endl;
            auto ivalue=input->value(iline,j,t);
            (*output)(iline,j,k)=ivalue;
        }
    }

    return ResultCode::Ok;
}
