#include "rockpropertiesprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

RockPropertiesProcess::RockPropertiesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Math"), project, parent){

}

ProjectProcess::ResultCode RockPropertiesProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_well=getParam(parameters,"well");
        m_dtName=getParam(parameters, "dt");
        m_dtsName=getParam(parameters, "dts");
        m_denName=getParam(parameters, "den");
        m_youngsModulusName=getParam(parameters, "youngs-modulus");
        m_bulkModulusName=getParam(parameters, "bulk-modulus");
        m_shearModulusName=getParam(parameters, "shear-modulus");
        m_poissonRatioName=getParam(parameters, "poisson-ratio");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode RockPropertiesProcess::run(){

    emit currentTask(tr("Loading input logs"));
    emit started(3);
    qApp->processEvents();

    auto dtlog=project()->loadLog( m_well, m_dtName);
    if( !dtlog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(m_well, m_dtName));
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    std::shared_ptr<Log> dtslog=project()->loadLog( m_well, m_dtsName);
    if( !dtslog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(m_well, m_dtsName));
        return ResultCode::Error;
    }
    emit progress(2);
    qApp->processEvents();

    std::shared_ptr<Log> denlog=project()->loadLog( m_well, m_denName);
    if( !denlog){
        setErrorString(QString("Loading log \"%1-%2\" failed!").arg(m_well, m_denName));
        return ResultCode::Error;
    }
    emit progress(3);
    qApp->processEvents();

    if( dtlog->nz()!=dtslog->nz() || dtlog->nz()!=denlog->nz() ){
        setErrorString("Input logs contain different number of samples!");
        return ResultCode::Error;
    }

    auto ymlog=std::make_shared<Log>( m_youngsModulusName.toStdString(), "MPSI", "Youngs Modulus",
                                       dtlog->z0(), dtlog->dz(), dtlog->nz() );
    if( !ymlog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    auto bmlog=std::make_shared<Log>( m_bulkModulusName.toStdString(), "MPSI", "Bulk Modulus",
                                       dtlog->z0(), dtlog->dz(), dtlog->nz() );
    if( !bmlog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    auto smlog=std::make_shared<Log>( m_shearModulusName.toStdString(), "MPSI", "Shear Modulus",
                                       dtlog->z0(), dtlog->dz(), dtlog->nz() );
    if( !smlog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    auto prlog=std::make_shared<Log>( m_poissonRatioName.toStdString(), "", "Poisson Ratio",
                                       dtlog->z0(), dtlog->dz(), dtlog->nz() );
    if( !prlog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    emit currentTask(tr("Computing logs"));
    emit started(dtlog->nz());
    qApp->processEvents();

    const double U=13400.;	// this is for english units (ft) - den in g/cm^3 -> mpsi; use 1000 for metric
    for( int i=0; i<dtlog->nz(); i++){

        auto dt=(*dtlog)[i];
        if(dt==dtlog->NULL_VALUE) continue;

        auto dts=(*dtslog)[i];
        if(dts==dtslog->NULL_VALUE) continue;

        auto den=(*denlog)[i];
        if(den==denlog->NULL_VALUE) continue;

        auto N=U*den/std::pow(dts,2);   						// shear modulus
        auto R=dts/dt;											// vp/vs
        auto PR=(0.5*std::pow(R,2)-1)/(std::pow(R,2)-1);		// poisson ratio
        auto B=U*den*(1./std::pow(dt,2)-4./std::pow(dts,2)/3);	// bulk modulus
        auto Y=2*N*(1.+PR);										// youngs modulus

        (*ymlog)[i]=Y;
        (*bmlog)[i]=B;
        (*smlog)[i]=N;
        (*prlog)[i]=PR;

        if(i%1000==0){
            emit progress(i);
            qApp->processEvents();
        }
    }

    emit currentTask(tr("Saving logs"));
    emit started(4);
    qApp->processEvents();

    if( !project()->addLog( m_well, m_youngsModulusName, *ymlog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(m_well, m_youngsModulusName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    if( !project()->addLog( m_well, m_bulkModulusName, *bmlog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(m_well, m_bulkModulusName) );
        return ResultCode::Error;
    }
    emit progress(2);
    qApp->processEvents();

    if( !project()->addLog( m_well, m_shearModulusName, *smlog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(m_well, m_shearModulusName) );
        return ResultCode::Error;
    }
    emit progress(3);
    qApp->processEvents();

    if( !project()->addLog( m_well, m_poissonRatioName, *prlog ) ){
        setErrorString( QString("Could not add log \"%1 - %2\" to project!").arg(m_well, m_poissonRatioName) );
        return ResultCode::Error;
    }
    emit progress(4);
    qApp->processEvents();

    return ResultCode::Ok;
}

