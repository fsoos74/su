#include "horizontotopprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include<topsdbmanager.h>
#include<wellpath.h>
#include<QMap>

HorizonToTopProcess::HorizonToTopProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Horizon To Top"), project, parent){

}

ProjectProcess::ResultCode HorizonToTopProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_wells=unpackParamList( getParam(parameters, "wells") );
        m_horizonName=getParam(parameters, "horizon");
        m_topName=getParam(parameters, "top");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}



ProjectProcess::ResultCode HorizonToTopProcess::run(){

    m_horizon=project()->loadGrid(GridType::Horizon, m_horizonName);
    if( !m_horizon ){
        setErrorString("Loading Horizon failed!");
        return ResultCode::Error;
    }

    project()->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    emit started(m_wells.size());
    qApp->processEvents();

    int i=0;
    for( auto well : m_wells){

        auto res=processWell(well);
        if( res!=ResultCode::Ok) return res;

        emit progress(++i);
        qApp->processEvents();

        if( isCanceled()) return ResultCode::Canceled;
    }

    return ResultCode::Ok;
}

#include<iostream>


ProjectProcess::ResultCode HorizonToTopProcess::processWell(QString well){

    auto wp=project()->loadWellPath(well);
    if( !wp ){
        setErrorString(QString("Loading well path for well \"%1\" failed!").arg(well));
        return ResultCode::Error;
    }

    auto zh=m_horizon->NULL_VALUE;
    for( int j=1; j<wp->size();j++){
        auto xyz=(*wp)[j];
        auto ilxl=xy_to_ilxl.map(QPointF(xyz[0], xyz[1]));
        zh=-m_horizon->valueAt(ilxl.x(), ilxl.y());
        //if( zh>-xyz[2]) // z==elevation
        if( zh==m_horizon->NULL_VALUE) continue;
        auto z1=xyz[2];
        if( zh<z1 ) continue;  // we are still above horizon
        // intersection with horizon is between j and j-1, do linear interpolation
        auto z0=(*wp)[j-1][2];
        auto w1=(zh-z0)/(z1-z0);
        zh=(1.-w1)*z0+w1*z1;
        zh=wp->mdAtZ(zh);
        break;
    }

    if( zh!=m_horizon->NULL_VALUE){

        auto db=project()->openTopsDatabase();
        db->insert(WellMarker(m_topName, well, zh));
    }

    return ResultCode::Ok;
}
