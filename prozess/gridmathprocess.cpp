#include "gridmathprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include<functional>
using namespace std::placeholders;

GridMathProcess::GridMathProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Crop Grid"), project, parent){

}

ProjectProcess::ResultCode GridMathProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("function"))){
        setErrorString("Parameters contain no function!");
        return ResultCode::Error;
    }
    int f=parameters.value(QString("function")).toInt();

    switch(f){

    case 0: m_func= std::bind(&GridMathProcess::add_gv, this, _1, _2) ; break;
    case 1: m_func= std::bind(&GridMathProcess::mul_gv, this, _1, _2) ; break;
    case 2: m_func= std::bind(&GridMathProcess::add_gg, this, _1, _2) ; break;
    case 3: m_func= std::bind(&GridMathProcess::mul_gg, this, _1, _2) ; break;

    default:
        setErrorString("Invalid function!");
        return ResultCode::Error;
        break;
    }

    if( parameters.contains(QString("value"))){
        m_value=parameters.value(QString("value")).toDouble();
    }

    if( !parameters.contains(QString("output-type"))){
        setErrorString("Parameters contain no output grid type!");
        return ResultCode::Error;
    }
    m_outputType = toGridType(parameters.value(QString("output-type") ) );

    if( !parameters.contains(QString("output-grid"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output-grid"));

    if( parameters.contains(QString("input-grid1-type"))){
        GridType type1=toGridType(parameters.value(QString("input-grid1-type")));
        if(!parameters.contains(QString("input-grid1"))){
            setErrorString("Parameters contain no input grid 1!");
            return ResultCode::Error;
        }
        m_inputGrid1=project()->loadGrid(type1, parameters.value(QString("input-grid1")));
        if( !m_inputGrid1){
            setErrorString(QString("Loading input grid 1 failed!"));
            return ResultCode::Error;
        }
    }

    if( parameters.contains(QString("input-grid2-type"))){
        GridType type2=toGridType(parameters.value(QString("input-grid2-type")));
        if(!parameters.contains(QString("input-grid2"))){
            setErrorString("Parameters contain no input grid 2!");
            return ResultCode::Error;
        }
        m_inputGrid2=project()->loadGrid(type2, parameters.value(QString("input-grid2")));
        if( !m_inputGrid2){
            setErrorString(QString("Loading input grid 1 failed!"));
            return ResultCode::Error;
        }
    }

    if( !m_inputGrid1 ){
        setErrorString(QString("No input grid loaded!!!"));
        return ResultCode::Error;
    }

    if( m_inputGrid2 && m_inputGrid1->bounds()!=m_inputGrid2->bounds() ){
        setErrorString(QString("Input grids have diffeent geometry!"));
        return ResultCode::Error;
    }

    m_outputGrid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(m_inputGrid1->bounds()));

    if( !m_outputGrid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

float GridMathProcess::add_gv(int i, int j){
    float v1=m_inputGrid1->valueAt(i,j);
    if( v1==m_inputGrid1->NULL_VALUE) return v1;
    return v1+m_value;
}

float GridMathProcess::mul_gv(int i, int j){
    float v1=m_inputGrid1->valueAt(i,j);
    if( v1==m_inputGrid1->NULL_VALUE) return v1;
    return v1*m_value;
}

float GridMathProcess::add_gg(int i, int j){
    float v1=m_inputGrid1->valueAt(i,j);
    if( v1==m_inputGrid1->NULL_VALUE) return v1;
    float v2=m_inputGrid2->valueAt(i,j);
    if( v2==m_inputGrid2->NULL_VALUE) return v2;
    return v1+v2;
}

float GridMathProcess::mul_gg(int i, int j){
    float v1=m_inputGrid1->valueAt(i,j);
    if( v1==m_inputGrid1->NULL_VALUE) return v1;
    float v2=m_inputGrid2->valueAt(i,j);
    if( v2==m_inputGrid2->NULL_VALUE) return v2;
    return v1*v2;
}

ProjectProcess::ResultCode GridMathProcess::run(){


    Grid2DBounds bounds=m_outputGrid->bounds();

    emit currentTask("Computing output grid");
    emit started(bounds.height());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            (*m_outputGrid)(i,j)=m_func( i, j );

        }

        emit progress( i-bounds.i1() );
        qApp->processEvents();

    }

    emit currentTask("Saving output grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addGrid( m_outputType, m_outputName, m_outputGrid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
