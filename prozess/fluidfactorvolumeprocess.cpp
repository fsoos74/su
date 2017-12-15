#include "fluidfactorvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>

#include<QThread>
#include<fstream>
#include<iostream>

FluidFactorVolumeProcess::FluidFactorVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Compute Fluid Factor"), project, parent){

}

ProjectProcess::ResultCode FluidFactorVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    if( !parameters.contains(QString("volume"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("volume"));



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


    if( !(m_intercept->bounds()==m_gradient->bounds()) ){
        setErrorString("Intercept and gradient volumes have different geometry!");
        return ResultCode::Error;
    }

    Grid3DBounds bounds=m_intercept->bounds();
    m_volume=std::shared_ptr<Volume >( new Volume(bounds));


    if( parameters.contains("angle") ){
        m_computeAngle=false;
        m_angle=parameters.value(QString("angle")).toDouble(); // in degrees
        m_angle*=M_PI/180.0;    // convert to radian
    }
    else{
        m_computeAngle=true;
    }

    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode FluidFactorVolumeProcess::run(){


    // first step compute crossplot angle
    Grid3DBounds bounds=m_intercept->bounds();      // intercept and gradient have same bounds, checked on init

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    if( m_computeAngle ){

    QVector<QPointF> all;

    emit currentTask("Computing crossplot angle");
    emit started(100);

    qApp->processEvents();

    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        for( int xline=bounds.j1(); xline<=bounds.j2(); xline++){

            for( int sample=0; sample<bounds.nt(); sample++){

                float intercept=(*m_intercept)(iline, xline, sample);
                if( intercept == m_intercept->NULL_VALUE) continue;


                float gradient=(*m_gradient)(iline, xline, sample);
                if( gradient==m_gradient->NULL_VALUE) continue;


                all.push_back( QPointF(intercept, gradient) );

            }
        }

        if( (iline-bounds.i1()) % onePercent==0 ){
            emit progress((iline-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }



    QPointF trendInterceptAndGradient=linearRegression(all);
    m_angle=std::fabs(std::atan( trendInterceptAndGradient.y() ) );
    }
std::cout<<"angle="<<m_angle<<std::endl<<std::flush;
    double phi=m_angle;
    double sinPhi=std::sin(phi);
    double cosPhi=std::cos(phi);

    emit currentTask("Computing crossplot fluid factors");
    emit started(100);
    qApp->processEvents();

    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        for( int xline=bounds.j1(); xline<=bounds.j2(); xline++){

            for( int sample=0; sample<bounds.nt(); sample++){

                float intercept=(*m_intercept)(iline, xline, sample);
                if( intercept == m_intercept->NULL_VALUE) continue;

                float gradient=(*m_gradient)(iline, xline, sample);
                if( gradient==m_gradient->NULL_VALUE) continue;

                double fluidFactor=intercept*sinPhi + gradient*cosPhi;
                (*m_volume)(iline, xline, sample)=fluidFactor;

            }
        }

        if( (iline-bounds.i1()) % onePercent==0 ){
            emit progress((iline-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_volumeName, m_volume, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
     qApp->processEvents();


    return ResultCode::Ok;
}


