#include "secondaryattributesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

SecondaryAttributesProcess::SecondaryAttributesProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Secondary Attributes"), project, parent){

}

ProjectProcess::ResultCode SecondaryAttributesProcess::init( const QMap<QString, QString>& parameters ){

    using namespace std::placeholders;

    if( !parameters.contains(QString("output"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output"));


    if( !parameters.contains(QString("intercept"))){

        setErrorString("Parameters contain no intercept grid!");
        return ResultCode::Error;
    }
    m_interceptName=parameters.value( QString("intercept") );

    m_intercept=project()->loadGrid( GridType::Attribute, m_interceptName);
    if( !m_intercept ){
        setErrorString("Loading intercept grid failed!");
        return ResultCode::Error;
    }



    if( !parameters.contains(QString("gradient"))){

        setErrorString("Parameters contain no gradient grid!");
        return ResultCode::Error;
    }

    m_gradientName=parameters.value( QString("gradient") );
    m_gradient=project()->loadGrid( GridType::Attribute, m_gradientName);
    if( !m_gradient ){
        setErrorString("Loading gradient grid failed!");
        return ResultCode::Error;
    }

    
    if( !parameters.contains(QString("type"))){

        setErrorString("Parameters contain no type!");
        return ResultCode::Error;
    }
    int type=parameters.value("type").toInt();

    switch(type){

    case 0: m_func= std::bind(&SecondaryAttributesProcess::GxI, this, _1, _2) ; break;
    case 1: m_func= std::bind(&SecondaryAttributesProcess::IqG, this, _1, _2) ; break;
    case 2:
            m_func=std::bind(&SecondaryAttributesProcess::GxIxFF, this, _1, _2) ;
            if( !parameters.contains("fluid-factor")){
                setErrorString("Parameters contain fluid factor!");
                return ResultCode::Error;
            }
            m_fluidFactorName=parameters.value( QString("fluid-factor") );


            m_fluidFactor=project()->loadGrid( GridType::Attribute, m_fluidFactorName);
            if( !m_fluidFactor ){
                setErrorString("Loading Fluid Factor grid failed!");
                return ResultCode::Error;
            }

            if( !(m_intercept->bounds()==m_fluidFactor->bounds()) ){
                setErrorString("Intercept and Fluid Factor grids have different geometry!");
                return ResultCode::Error;
            }

        break;
    }

    if( !(m_intercept->bounds()==m_gradient->bounds()) ){
        setErrorString("Intercept and gradient grids have different geometry!");
        return ResultCode::Error;
    }



    Grid2DBounds bounds=m_intercept->bounds();
    m_grid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));


    if( !m_grid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode SecondaryAttributesProcess::run(){


    Grid2DBounds bounds=m_intercept->bounds();      // iall input grids have same bounds, checked on init
    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;


    emit currentTask("Computing output grid");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            (*m_grid)(i,j)=m_func(i,j);
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_outputName );
    if( !project()->addGrid( gridTypeAndName.first, gridTypeAndName.second, m_grid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}


float SecondaryAttributesProcess::GxI(int il,int xl){

    float G=(*m_gradient)(il,xl);
    float I=(*m_intercept)(il,xl);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE){
        return m_grid->NULL_VALUE;
    }

    return G*I;
}

float SecondaryAttributesProcess::IqG(int il, int xl){

    float G=(*m_gradient)(il,xl);
    float I=(*m_intercept)(il,xl);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE
            || G==0 ){                          // better use eps
        return m_grid->NULL_VALUE;
    }

    return I/G;
}

float SecondaryAttributesProcess::GxIxFF(int il, int xl){

    float G=(*m_gradient)(il,xl);
    float I=(*m_intercept)(il,xl);
    float FF=(*m_fluidFactor)(il,xl);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE
            || FF==m_fluidFactor->NULL_VALUE ){
        return m_grid->NULL_VALUE;
    }

    return G*I*FF;
}
