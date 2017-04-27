#include "horizoncurvatureprocess.h"

#include<QApplication>

#include<cmath>
using std::pow;
using std::sqrt;

HorizonCurvatureProcess::HorizonCurvatureProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Horizon Curvature"), project, parent){

}

ProjectProcess::ResultCode HorizonCurvatureProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("basename"))){

        setErrorString("Parameters contain no basename!");
        return ResultCode::Error;
    }
    m_baseName=parameters.value( QString("basename") );

    QString attributes[]={ MEAN_CURVATURE_STR, GAUSS_CURVATURE_STR, MIN_CURVATURE_STR, MAX_CURVATURE_STR,
                           MAX_POS_CURVATURE_STR, MAX_NEG_CURVATURE_STR, DIP_CURVATURE_STR, STRIKE_CURVATURE_STR };
    for( auto attr : attributes){
        if( parameters.contains(attr) && parameters.value(attr).toInt()>0){ // positive is assumed true
            m_output_attributes.insert(attr);
        }
    }


    if( !parameters.contains(QString("horizon"))){

        setErrorString("Parameters contain no horizon!");
        return ResultCode::Error;
    }
    m_horizonName=parameters.value( QString("horizon") );
    m_horizon=project()->loadGrid( GridType::Horizon, m_horizonName);
    if( !m_horizon ){
        setErrorString("Loading horizon failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

// computation according to paper from Andy Roberts, Enterprise Oil
ProjectProcess::ResultCode HorizonCurvatureProcess::run(){

    Grid2DBounds bounds=m_horizon->bounds();

    std::shared_ptr<Grid2D<float>> gmean( new Grid2D<float>(bounds) );
    std::shared_ptr<Grid2D<float>> ggauss( new Grid2D<float>(bounds) );
    std::shared_ptr<Grid2D<float>> gmin( new Grid2D<float>(bounds) );
    std::shared_ptr<Grid2D<float>> gmax( new Grid2D<float>(bounds) );
    std::shared_ptr<Grid2D<float>> gpos( new Grid2D<float>(bounds) );
    std::shared_ptr<Grid2D<float>> gneg( new Grid2D<float>(bounds) );

    emit started(bounds.height()-2); // don't use first/last row/column because we need 8 surrounding cdp for each output cdp
    qApp->processEvents();

    for( int i=bounds.i1()+1; i<=bounds.i2()-1; i++){

        for( int j=bounds.j1()+1; j<=bounds.j2()-1; j++){

            // XXX need to handle NULL values

            float Z1=(*m_horizon)(i-1, j-1);
            float Z2=(*m_horizon)(i-1, j);
            float Z3=(*m_horizon)(i-1, j+1);

            float Z4=(*m_horizon)(i, j-1);
            float Z5=(*m_horizon)(i, j);
            float Z6=(*m_horizon)(i, j+1);

            float Z7=(*m_horizon)(i+1, j-1);
            float Z8=(*m_horizon)(i+1, j);
            float Z9=(*m_horizon)(i+1, j+1);

            if( Z1==m_horizon->NULL_VALUE || Z2==m_horizon->NULL_VALUE || Z3==m_horizon->NULL_VALUE ||
                Z4==m_horizon->NULL_VALUE || Z5==m_horizon->NULL_VALUE || Z6==m_horizon->NULL_VALUE ||
                Z7==m_horizon->NULL_VALUE || Z8==m_horizon->NULL_VALUE || Z9==m_horizon->NULL_VALUE) continue;

            float a = (Z1+Z3+Z4+Z6+Z7+Z9)/12 - (Z2+Z5+Z8)/6;
            float b = (Z1+Z2+Z3+Z7+Z8+Z9)/12 - (Z4+Z5+Z6)/6;
            float c = (Z3+Z7-Z1-Z9)/4;
            float d = (Z3+Z6+Z9-Z1-Z4-Z7)/6;
            float e = (Z1+Z2+Z3-Z7-Z8-Z9)/6;
            float f = ( 2*(Z2+Z4+Z6+Z8) - (Z1+Z3+Z7+Z9) +5*Z5) / 9;
//std::cout<<i<<" "<<j<<" "<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<std::endl;
            float kmean  = ( a*(1+e*e) -c*d*e + b*(1+d*d) ) / std::pow( 1 + d*d + e*e, 1.5);
            float kgauss = ( 4*a*b - c*c) / std::pow( 1+d*d+e*e, 2);
            float kmax = kmean + sqrt( kmean*kmean - kgauss );
            float kmin = kmean - sqrt( kmean*kmean - kgauss );
            float kpos = a+b+sqrt( std::pow(a-b, 2) + c*c );
            float kneg = a+b-sqrt( std::pow(a-b, 2) + c*c );

            (*gmean)(i,j)=kmean;
            (*ggauss)(i,j)=kgauss;
            (*gmax)(i,j)=kmax;
            (*gmin)(i,j)=kmin;//kmin;
            (*gpos)(i,j)=kpos;
            (*gneg)(i,j)=kneg;
        }

        emit progress(i-1);
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    // write output grids
    // need to add check whether those grids exist and reserve in init function!!!

    if( m_output_attributes.contains(MIN_CURVATURE_STR)){
        QString name=QString("%1-minimum").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, gmin ) ) {
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_CURVATURE_STR)){
        QString name=QString("%1-maximum").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, gmax ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MEAN_CURVATURE_STR)){
        QString name=QString("%1-mean").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, gmean ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(GAUSS_CURVATURE_STR)){
        QString name=QString("%1-gaussian").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, ggauss ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_POS_CURVATURE_STR)){
        QString name=QString("%1-maximum-positive").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, gpos ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_NEG_CURVATURE_STR)){
        QString name=QString("%1-maximum-negative").arg(m_baseName);
        if( !project()->addGrid( GridType::Other, name, gneg ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    emit finished();

    return ResultCode::Ok;
}


