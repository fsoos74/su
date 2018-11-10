#include "volumetdconversionprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<QApplication>
#include<QMap>
#include<omp.h>

namespace {

QMap<TDDirection,QString> tdlookup={
    {TDDirection::TIME_TO_DEPTH, "Time to Depth"},
    {TDDirection::DEPTH_TO_TIME, "Depth to Time"}
};

}

QString toQString(TDDirection d){
    return tdlookup.value(d);
}

TDDirection toTDDirection(QString str){
    return tdlookup.key(str);
}



VolumeTDConversionProcess::VolumeTDConversionProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Volume TD Conversion"), project, parent){

}

ProjectProcess::ResultCode VolumeTDConversionProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString oname;
    QString iname;
    QString vname;
    int idz;
    int ilz;

    try{
        m_direction=toTDDirection( getParam(parameters, "direction"));
        oname=getParam(parameters, "output-volume");
        iname=getParam(parameters, "input-volume");
        vname=getParam(parameters, "velocity-volume");
        idz=getParam( parameters, "dz").toInt();
        ilz=getParam( parameters, "lz").toInt();
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
    /*
    // output volume sampling from velocity volume, assume vels start at 0!!!
    auto z0=vbounds.ft();
    auto dz=vbounds.dt();
    auto nz=vbounds.nt();
    */
    auto dz=0.001*idz;	// convert from millis
    auto nz=static_cast<int>(std::ceil(double(ilz)/idz))+1;
    auto obounds=Grid3DBounds(i1,i2, j1,j2, nz, 0, dz );
    setBounds(obounds);

    res=addOutputVolume(oname, bounds(), Domain::Depth, inputType(0));
    if( res!=ResultCode::Ok) return res;

    return ResultCode::Ok;
}


// assume irregular sorted ascending x
QVector<float> regularize( QVector<std::pair<float,float>> irregular,
                                float dx, int nx, float fill=0){

    QVector<float> regular(nx,fill);

    int ii=0;

    for( int ir=0; ir<regular.size(); ir++){
        float xir=ir*dx;
        if( xir<irregular.front().first ) continue;     // no extrapolation!!
        while( (ii+1<irregular.size()) && (irregular[ii+1].first<xir) ) ii++;
        if( ii+1 >= irregular.size()) break;
        // linear interpolation
        auto x0=irregular[ii].first;
        auto v0=irregular[ii].second;
        auto x1=irregular[ii+1].first;
        auto v1=irregular[ii+1].second;
        auto fac1=(xir-x0)/(x1-x0);
        auto v=(1.-fac1)*v0 + fac1*v1;
        regular[ir]=v;
    }

    return regular;
}



ProjectProcess::ResultCode VolumeTDConversionProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){
    std::shared_ptr<Volume> input=inputs[0];
    std::shared_ptr<Volume> ivel=inputs[1];
    std::shared_ptr<Volume> output=outputs[0];

    //compute average velocity in depth
    auto vad=std::make_shared<Volume>(bounds(), Domain::Depth, VolumeType::AVEL, 0);
#pragma omp parallel for
    for( int j=bounds().j1(); j<=bounds().j2(); j++){
        double sum=0;
        for( int k=0; k<bounds().nt(); k++){
             sum+=(*ivel)(iline,j,k);
             (*vad)(iline,j,k)=sum/(k+1);
        }
    }

    if( m_direction==TDDirection::TIME_TO_DEPTH){

    // convert samples
#pragma omp parallel for
    for( int j=bounds().j1(); j<=bounds().j2(); j++){
        for( int k=0; k<bounds().nt(); k++){
            auto z=bounds().sampleToTime(k);		// Time in sampling is depth for depth data!!!
            auto v=(*vad)(iline,j,k);
            auto t=2*1000*z/v;						// TIME IS TWO WAY TIME
            auto ivalue=input->value(iline,j,t);
            (*output)(iline,j,k)=ivalue;
        }
    }

    }
    else{           // depth to time

#pragma omp parallel for
    for( int j=bounds().j1(); j<=bounds().j2(); j++){
        auto ibounds=input->bounds();	// depth
        QVector<std::pair<float,float>> irr(ibounds.nt());
        for( int k=0; k<ibounds.nt(); k++){
            auto z=ibounds.sampleToTime(k);
            auto v=(*vad)(iline,j,k);
            float t=2*1000*z/v;						// TWT
            auto value=(*input)(iline,j,k);
            irr[k]=std::make_pair(t, value);
        }

        auto reg=regularize(irr, bounds().dt(), bounds().nt(), output->NULL_VALUE);
        for( int k=0; k<bounds().nt(); k++){
            (*output)(iline,j,k)=reg[k];
        }
    }

    }

    return ResultCode::Ok;
}
