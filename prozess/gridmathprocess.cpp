#include "gridmathprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include<functional>
using namespace std::placeholders;

GridMathProcess::GridMathProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Grid Math"), project, parent){

}

ProjectProcess::ResultCode GridMathProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    QString iname1;
    GridType itype1;
    QString iname2;
    GridType itype2;
    double value=0;


    try{
        func=getParam(parameters, "function");

        m_outputName=getParam(parameters, "output-name");
        auto ots=getParam(parameters, "output-type");
        m_outputType=toGridType(ots);

        iname1=getParam(parameters, "input-grid1");
        auto it1s=getParam(parameters, "input-grid1-type");
        itype1=toGridType(it1s);

        iname2=getParam(parameters, "input-grid2");
        auto it2s=getParam(parameters, "input-grid2-type");
        itype2=toGridType(it2s);

        value=getParam(parameters, "value").toDouble();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    std::cout<<"path="<<project()->getGridPath(itype1,iname1).toStdString()<<std::endl<<std::flush;
    m_inputGrid1=project()->loadGrid(itype1, iname1);
    if( !m_inputGrid1){
        setErrorString(QString("Loading grid \"%1\" failed!").arg(iname1));
        return ResultCode::Error;
    }
    auto bounds=m_inputGrid1->bounds();
    std::cout<<"il="<<bounds.i1()<<"-"<<bounds.i2()<<" xl="<<bounds.j1()<<"-"<<bounds.j2()<<" NULL="<<m_inputGrid1->NULL_VALUE<<std::endl<<std::flush;


    if( !iname2.isEmpty()){
        m_inputGrid2=project()->loadGrid(itype2, iname2);
        if( !m_inputGrid2){
            setErrorString(QString("Loading grid \"%2\" failed!").arg(iname1));
            return ResultCode::Error;
        }
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

    m_processor.setOP(MathProcessor::toOP(func));
    m_processor.setValue(value);
    m_processor.setInputNullValue(m_inputGrid1->NULL_VALUE);

    return ResultCode::Ok;
}


ProjectProcess::ResultCode GridMathProcess::run(){


    Grid2DBounds bounds=m_outputGrid->bounds();

    emit currentTask("Computing output grid");
    emit started(bounds.height());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            auto ivalue1=(*m_inputGrid1)(i,j);
            m_processor.setInput1(ivalue1);

            if(m_inputGrid2){
                auto ivalue2=(*m_inputGrid2)(i,j);
                m_processor.setInput2(ivalue2);
            }

            auto res=m_processor.compute();

            if( res!=m_processor.NULL_VALUE) (*m_outputGrid)(i,j)=res;

        }

        emit progress( i-bounds.i1() );
        qApp->processEvents();

    }

    emit currentTask("Saving output grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addGrid( m_outputType, m_outputName, m_outputGrid, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
