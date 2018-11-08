#include "createvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

CreateVolumeProcess::CreateVolumeProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Create Volume"), project, parent){

}


ProjectProcess::ResultCode CreateVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    int minZ;
    int maxZ;
    int dz;
    QString name;
    Domain domain;
    VolumeType type;

    try{

        name=getParam(parameters, "volume");
        minIline=getParam(parameters, "min-iline").toInt();
        maxIline=getParam( parameters, "max-iline").toInt();
        minXline=getParam( parameters, "min-xline").toInt();
        maxXline=getParam( parameters, "max-xline").toInt();
        minZ=getParam( parameters, "min-z").toInt();
        maxZ=getParam( parameters, "max-z").toInt();
        dz=getParam( parameters, "dz").toInt();
        auto domain_s=getParam( parameters, "domain");
        domain=toDomain(domain_s);
        auto type_s=getParam( parameters, "type");
        type=toVolumeType(type_s);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    int nz=1 + ( maxZ - minZ )/dz;
    auto b=Grid3DBounds(minIline, maxIline, minXline, maxXline, nz, 0.001*minZ, 0.001*dz);

    setBounds(b);

    auto res=addOutputVolume( name, b, domain, type);
    if( res!=ResultCode::Ok) return res;

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CreateVolumeProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    auto output=outputs[0];
    auto input=inputs[0];
    for( int j=bounds().j1(); j<bounds().j2(); j++){
        for(int k=0; k<bounds().nt(); k++){
            (*output)(iline,j,k)=(*input)(iline,j,k);
        }
    }

    return ResultCode::Ok;
}
