#include "flattenvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>
#include <QMap>
#include<iostream>

#include<QApplication>

namespace{

QMap<FlattenVolumeProcess::Mode, QString> lookup{
    { FlattenVolumeProcess::Mode::Flatten, "Flatten"},
    { FlattenVolumeProcess::Mode::Unflatten, "Unflatten"}
};

}

QString FlattenVolumeProcess::toQString(FlattenVolumeProcess::Mode m){
    return lookup.value(m);
}

FlattenVolumeProcess::Mode FlattenVolumeProcess::toMode(QString str){
    return lookup.key(str);
}


FlattenVolumeProcess::FlattenVolumeProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Flatten Volume"), project, parent){

}

ProjectProcess::ResultCode FlattenVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString outputName;
    QString inputName;
    QString horizonName;

    try{
        outputName=getParam(parameters, "output-volume");
        inputName=getParam(parameters, "input-volume");
        horizonName=getParam(parameters, "horizon" );
        m_start=getParam(parameters, "start").toInt();
        m_length=getParam(parameters, "length").toInt();
        m_mode=toMode(getParam(parameters, "mode"));
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    ProjectProcess::ResultCode res=addInputVolume(inputName);
    if(res!=ResultCode::Ok) return res;

    auto ibounds=inputBounds(0);
    int ont = static_cast<int>(1 + 0.001*m_length / ibounds.dt());  // length is in millis
    float oft=0.001*m_start;        //msec -> sec
    auto obounds=Grid3DBounds( ibounds.i1(), ibounds.i2(), ibounds.j1(), ibounds.j2(), ont, oft, ibounds.dt());
    setBounds(obounds);

    res=addOutputVolume(outputName, obounds, inputDomain(0), inputType(0) );
    if( res!=ResultCode::Ok) return res;

    m_horizon=project()->loadGrid(GridType::Horizon, horizonName);
    if( !m_horizon){
        setErrorString(QString("Loading horizon \"%1\" failed!").arg(horizonName));
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode FlattenVolumeProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    std::shared_ptr<Volume> input=inputs[0];
    std::shared_ptr<Volume> output=outputs[0];

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        auto thms=m_horizon->valueAt(iline,j);
        if( thms==m_horizon->NULL_VALUE) continue;
        auto th = 0.001*thms;

        if( m_mode==Mode::Flatten){
            for( int k=0; k<bounds().nt(); k++){
                (*output)(iline,j,k)=input->value( iline, j, th + k*bounds().dt());
            }
        }
        else{   // unflatten
            for( int k=0; k<bounds().nt(); k++){
                auto tk = bounds().sampleToTime(k);
                auto tflat = tk - th;
                (*output)(iline,j,k)=input->value( iline, j, tflat);  // this doesinterpolation
            }
        }
    }

    return ResultCode::Ok;
}
