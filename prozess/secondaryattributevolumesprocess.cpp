#include "secondaryattributevolumesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

SecondaryAttributeVolumesProcess::SecondaryAttributeVolumesProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Secondary Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode SecondaryAttributeVolumesProcess::init( const QMap<QString, QString>& parameters ){

    using namespace std::placeholders;

    if( !parameters.contains(QString("output"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output"));


    if( !parameters.contains(QString("intercept"))){

        setErrorString("Parameters contain no intercept volume!");
        return ResultCode::Error;
    }
    m_interceptName=parameters.value( QString("intercept") );

    m_intercept=project()->loadVolume( m_interceptName);
    if( !m_intercept ){
        setErrorString("Loading intercept volume failed!");
        return ResultCode::Error;
    }



    if( !parameters.contains(QString("gradient"))){

        setErrorString("Parameters contain no gradient volume!");
        return ResultCode::Error;
    }

    m_gradientName=parameters.value( QString("gradient") );
    m_gradient=project()->loadVolume( m_gradientName);
    if( !m_gradient ){
        setErrorString("Loading gradient volume failed!");
        return ResultCode::Error;
    }

    
    if( !parameters.contains(QString("type"))){

        setErrorString("Parameters contain no type!");
        return ResultCode::Error;
    }
    int type=parameters.value("type").toInt();

    switch(type){

    case 0: m_func= std::bind(&SecondaryAttributeVolumesProcess::GxI, this, _1, _2, _3) ; break;
    case 1: m_func= std::bind(&SecondaryAttributeVolumesProcess::IqG, this, _1, _2, _3) ; break;
    case 2:
            m_func=std::bind(&SecondaryAttributeVolumesProcess::GxIxFF, this, _1, _2, _3) ;
            if( !parameters.contains("fluid-factor")){
                setErrorString("Parameters contain fluid factor!");
                return ResultCode::Error;
            }
            m_fluidFactorName=parameters.value( QString("fluid-factor") );


            m_fluidFactor=project()->loadVolume( m_fluidFactorName);
            if( !m_fluidFactor ){
                setErrorString("Loading Fluid Factor volume failed!");
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



    Grid3DBounds bounds=m_intercept->bounds();
    m_volume=std::shared_ptr<Grid3D<float> >( new Grid3D<float>(bounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode SecondaryAttributeVolumesProcess::run(){


    Grid3DBounds bounds=m_intercept->bounds();      // iall input grids have same bounds, checked on init
    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;


    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1(); i<=bounds.inline2(); i++){

        for( int j=bounds.crossline1(); j<=bounds.crossline2(); j++){

            for( int k=0; k<bounds.sampleCount(); k++){
                (*m_volume)(i,j,k)=m_func(i,j,k);
            }
        }

        if( (i-bounds.inline1()) % onePercent==0 ){
            emit progress((i-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}


float SecondaryAttributeVolumesProcess::GxI(int il,int xl, int k){

    float G=(*m_gradient)(il,xl,k);
    float I=(*m_intercept)(il,xl,k);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE){
        return m_volume->NULL_VALUE;
    }

    return G*I;
}

float SecondaryAttributeVolumesProcess::IqG(int il, int xl, int k){

    float G=(*m_gradient)(il,xl,k);
    float I=(*m_intercept)(il,xl,k);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE
            || G==0 ){                          // better use eps
        return m_volume->NULL_VALUE;
    }

    return I/G;
}

float SecondaryAttributeVolumesProcess::GxIxFF(int il, int xl, int k){

    float G=(*m_gradient)(il,xl,k);
    float I=(*m_intercept)(il,xl,k);
    float FF=(*m_fluidFactor)(il,xl,k);

    if( G==m_gradient->NULL_VALUE
            || I==m_intercept->NULL_VALUE
            || FF==m_fluidFactor->NULL_VALUE ){
        return m_volume->NULL_VALUE;
    }

    return G*I*FF;
}
