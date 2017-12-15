#include "fluidfactorprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

FluidFactorProcess::FluidFactorProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Compute Fluid Factor"), project, parent){

}

ProjectProcess::ResultCode FluidFactorProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    if( !parameters.contains(QString("grid"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_gridName=parameters.value(QString("grid"));


    if( parameters.contains("angle") ){
        m_computeAngle=false;
        m_angle=parameters.value(QString("angle")).toDouble(); // in degrees
        m_angle*=M_PI/180.0;    // convert to radian
    }
    else{
        m_computeAngle=true;
    }

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

ProjectProcess::ResultCode FluidFactorProcess::run(){



    Grid2DBounds bounds=m_intercept->bounds();      // intercept and gradient have same bounds, checked on init
    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;

    if( m_computeAngle){

    // first step compute crossplot angle
    emit currentTask("Computing crossplot angle");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            double intercept=(*m_intercept)(i,j);
            if( intercept == m_intercept->NULL_VALUE) continue;

            double gradient=(*m_gradient)(i,j);
            if( gradient==m_gradient->NULL_VALUE) continue;

            all.push_back( QPointF(intercept, gradient) );
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    QPointF trendInterceptAndGradient=linearRegression(all);
    std::cout<<"ff compute angle: trend: x="<<trendInterceptAndGradient.x();
    std::cout<<" y="<<trendInterceptAndGradient.y()<<std::endl;
    m_angle=std::fabs(std::atan(trendInterceptAndGradient.y() ) );
    std::cout<<"angle="<<m_angle<<std::endl;
    }

    double phi=m_angle;

    std::cout<<"PHI="<<phi<<std::endl;
    double sinPhi=std::sin(phi);
    double cosPhi=std::cos(phi);

    emit currentTask("Computing crossplot fluid factors");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            double intercept=(*m_intercept)(i,j);
            if( intercept == m_intercept->NULL_VALUE) continue;

            double gradient=(*m_gradient)(i,j);
            if( gradient==m_gradient->NULL_VALUE) continue;

            double fluidFactor=intercept*sinPhi + gradient*cosPhi;
            (*m_grid)(i,j)=fluidFactor;
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
    std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_gridName );
    if( !project()->addGrid( gridTypeAndName.first, gridTypeAndName.second, m_grid, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_gridName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}


