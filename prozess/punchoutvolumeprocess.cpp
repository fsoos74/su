#include "punchoutvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<iostream>

PunchOutVolumeProcess::PunchOutVolumeProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Punch Out Volume"), project, parent){

}

ProjectProcess::ResultCode PunchOutVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString outputName;
    QString inputName;

    try{
        outputName=getParam(parameters, "output-volume");
        inputName=getParam(parameters, "input-volume");
        m_tableName=getParam(parameters, "points-table");
        m_keepPoints=static_cast<bool>(getParam(parameters, "keep-points").toInt());
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    auto res=addInputVolume(inputName);
    if( res!=ResultCode::Ok ) return res;

    auto bounds=inputBounds(0);
    setBounds(bounds);

    m_table=project()->loadTable(m_tableName);
    if( !m_table ){
        setErrorString(QString("Loading table \"%1\" failed!").arg(m_tableName));
        return ResultCode::Error;
    }

    res=addOutputVolume(outputName, bounds, inputDomain(0), inputType(0));
    if( res!=ResultCode::Ok) return res;

    return ResultCode::Ok;
}

ProjectProcess::ResultCode PunchOutVolumeProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){


    auto input=inputs[0];
    auto output=outputs[0];

    // first step copy input to output or fill output with null
    if(m_keepPoints){   // fill with null, next step will copy the points within the body from input
        for( int j=bounds().j1(); j<=bounds().j2(); j++){
            for( int k=0; k<bounds().nt(); k++){
                (*output)(iline,j,k)=output->NULL_VALUE;
            }
        }
    }
    else{               // copy input to output, next step will null out points within geobody
        for( int j=bounds().j1(); j<=bounds().j2(); j++){
            for( int k=0; k<bounds().nt(); k++){
                (*output)(iline,j,k)=(*input)(iline,j,k);
            }
        }
    }

    // second step assign null or copy from input
    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        if(!m_table->contains(iline,j)) continue;

        auto times=m_table->values(iline,j);

        if( m_keepPoints){
            for( auto t : times){
                auto k=bounds().timeToSample(t);
                (*output)(iline,j,k)=(*input)(iline,j,k);
            }
        }else{
            for( auto t : times){
                auto k=bounds().timeToSample(t);
                (*output)(iline,j,k)=output->NULL_VALUE;
            }
        }

    }

    return ResultCode::Ok;
}
