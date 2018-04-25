#include "tabletohorizonprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include<grid2d.h>
#include<table.h>
#include<QMap>
#include<memory>

TableToHorizonProcess::TableToHorizonProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Horizon To Top"), project, parent){

}

ProjectProcess::ResultCode TableToHorizonProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_horizonName=getParam(parameters, "horizon");
        m_tableName=getParam(parameters, "table");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}



ProjectProcess::ResultCode TableToHorizonProcess::run(){

    auto table=project()->loadTable(m_tableName);
    if( !table ){
        setErrorString("Loading Table failed!");
        return ResultCode::Error;
    }

    if(table->isMulti()){
        setErrorString("Tables with multiple values per cdp cannot be converted to horizon!");
        return ResultCode::Error;
    }

    if( table->size()<1){
        setErrorString("Input table is empty!");
        return ResultCode::Error;
    }

    emit started(table->size()*2);      // 2 passes
    qApp->processEvents();
    int n=0;

    // first pass find extends
    auto il0=std::numeric_limits<int>::max();
    auto il1=std::numeric_limits<int>::min();
    auto xl0=std::numeric_limits<int>::max();
    auto xl1=std::numeric_limits<int>::min();
    for( auto k12 : table->keys() ){
        auto il=k12.first;
        auto xl=k12.second;
        if(il<il0) il0=il;
        if(il>il1) il1=il;
        if(xl<xl0) xl0=xl;
        if(xl>xl1) xl1=xl;

        emit progress(++n);
        qApp->processEvents();

        if( isCanceled()) return ResultCode::Canceled;
    }

    //auto pbounds=project()->geometry().bboxLines();
    //Grid2DBounds hbounds(pbounds.left(), pbounds.top(), pbounds.right(), pbounds.bottom());
    Grid2DBounds hbounds(il0,xl0,il1,xl1);
    auto hrz=std::make_shared<Grid2D<float>>(hbounds);

    // second pass fill horizon
    for( auto k12 : table->keys() ){

       auto il=k12.first;
        auto xl=k12.second;
        if( hbounds.isInside(il,xl)){
            auto value=table->value(k12);
            (*hrz)(il,xl)=1000.*value;      // horizons are in millis
        }

        emit progress(++n);
        qApp->processEvents();

        if( isCanceled()) return ResultCode::Canceled;
    }

    if( !project()->addGrid(GridType::Horizon, m_horizonName, hrz, params())){
        setErrorString("Adding horizon to project failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
