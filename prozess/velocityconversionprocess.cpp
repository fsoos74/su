#include "velocityconversionprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<QApplication>
#include<QMap>
#include<omp.h>



VelocityConversionProcess::VelocityConversionProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Velocity Conversion"), project, parent){

}

ProjectProcess::ResultCode VelocityConversionProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString oname;
    QString iname;
    int idz;
    int ilz;
    VolumeType otype;

    try{
        otype=toVolumeType( getParam(parameters, "type"));
        oname=getParam(parameters, "output-volume");
        iname=getParam(parameters, "input-volume");
        idz=getParam( parameters, "dz").toInt();
        ilz=getParam( parameters, "lz").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    ProjectProcess::ResultCode res=addInputVolume(iname);
    if(res!=ResultCode::Ok) return res;

    auto ibounds=inputBounds(0);                // input volume
    auto itype=inputType(0);
    auto i1=ibounds.i1();
    auto i2=ibounds.i2();
    auto j1=ibounds.j1();
    auto j2=ibounds.j2();
    auto dz=ibounds.dt();//0.001*idz;	// convert from millis
    auto nz=ibounds.nt();//static_cast<int>(std::ceil(double(ilz)/idz))+1;
    auto obounds=Grid3DBounds(i1,i2, j1,j2, nz, 0, dz );
    auto odomain=inputDomain(0);				// keep domain
    setBounds(obounds);

    res=addOutputVolume(oname, bounds(), odomain, otype);
    if( res!=ResultCode::Ok) return res;

    m_ctype=Conversion::NoConversion;
    if(itype==VolumeType::IVEL && otype==VolumeType::AVEL){
        m_ctype=Conversion::IntervalToAverage;
    }
    else if(itype==VolumeType::AVEL && otype==VolumeType::IVEL){
        m_ctype=Conversion::AverageToInterval;
    }

    return ResultCode::Ok;
}

/*
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
*/


ProjectProcess::ResultCode VelocityConversionProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){
    std::shared_ptr<Volume> input=inputs[0];
    std::shared_ptr<Volume> output=outputs[0];

    if(m_ctype==Conversion::NoConversion){
        for( int j=bounds().j1(); j<=bounds().j2(); j++ ){
//#pragma omp parallel for
            for( int k=0; k<bounds().nt(); k++){
                 (*output)(iline,j,k)=(*input)(iline,j,k);
            }
        }
    }
    else if(m_ctype==Conversion::IntervalToAverage){
        double sum=0;
//#pragma omp parallel for
        for( int j=bounds().j1(); j<=bounds().j2(); j++ ){
            double t=0;
            for( int k=0; k<bounds().nt(); k++){
                 double ivel=(*input)(iline,j,k);
                 if(ivel==input->NULL_VALUE) break;
                 t+=bounds().dt()/ivel;		// dt()==fz!
                 double z=bounds().sampleToTime(k);
                 (*output)(iline,j,k)=z/t;
            }
        }
    }
    else if(m_ctype==Conversion::AverageToInterval){
//#pragma omp parallel for
        for( int j=bounds().j1(); j<=bounds().j2(); j++ ){
            double prev_t=0;
            for( int k=0; k<bounds().nt(); k++){
                 double avel=(*input)(iline,j,k);
                 if(avel==input->NULL_VALUE) break;
                 double z=bounds().sampleToTime(k);
                 double t=z/avel;
                 (*output)(iline,j,k)=bounds().dt() / (t-prev_t);

                 prev_t=t;
            }
        }
    }

    return ResultCode::Ok;
}
