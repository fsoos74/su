#include "curvaturevolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include <curvature_attributes.h> // names
#include<iostream>
#include<cmath>

CurvatureVolumeProcess::CurvatureVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Curvature Volume"), project, parent){

}

ProjectProcess::ResultCode CurvatureVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("basename"))){

        setErrorString("Parameters contain no basename!");
        return ResultCode::Error;
    }
    m_baseName=parameters.value( QString("basename") );

    m_maximumShift = parameters.value(QString("maximum-shift"), QString("7") ).toInt();
    m_windowLength = parameters.value(QString("window-length"), QString("5") ).toInt();

    QString attributes[]={ MEAN_CURVATURE_STR, GAUSS_CURVATURE_STR, MIN_CURVATURE_STR, MAX_CURVATURE_STR,
                           MAX_POS_CURVATURE_STR, MAX_NEG_CURVATURE_STR, DIP_CURVATURE_STR, STRIKE_CURVATURE_STR,
                           DIP_ANGLE_STR, DIP_AZIMUTH_STR};
    for( auto attr : attributes){
        if( parameters.contains(attr) && parameters.value(attr).toInt()>0){ // positive is assumed true
            m_output_attributes.insert(attr);
        }
    }


    if( !parameters.contains(QString("volume"))){

        setErrorString("Parameters contain no volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value( QString("volume") );
    m_volume=project()->loadVolume( m_volumeName);
    if( !m_volume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


// correlation of two sequences
template< class FIT1, class FIT2>
double corr( FIT1 first1, FIT1 last1, FIT2 first2 ){

    double sum=0;

    if( first1==last1 ) return sum;

    do{
        sum += *first1 * *first2;
        ++first1;
        ++first2;
    } while( first1!=last1 );

    return sum;
}

// returns how trc1 (i1,j1)  must be shifted (in samples) for best fit with trc2 (i2,j2) in wnd
int optimum_shift( const Grid3D<float>& vol, int i1, int j1, int i2, int j2,
            int wnd_start, int wnd_len=5, int max_abs_shift=5 ){

    auto trc1 = &vol(i1, j1, 0);
    auto trc2 = &vol(i2, j2, 0);

    int min_shift = std::max( -wnd_start, -max_abs_shift );
    int max_shift = std::min( max_abs_shift, vol.bounds().sampleCount() - wnd_start - wnd_len -1 );
    auto best_shift=min_shift;
    auto best_cor = corr( &trc1[wnd_start], &trc1[wnd_start+wnd_len],
                &trc2[wnd_start+best_shift] );
    for( auto shift = min_shift + 1; shift<=max_shift; shift++){
        auto cor = corr( &trc1[wnd_start], &trc1[wnd_start+wnd_len],
                &trc2[wnd_start+shift] );
        if( cor>best_cor){
            best_cor=cor;
            best_shift=shift;
        }
    }

    return best_shift;
}


ProjectProcess::ResultCode CurvatureVolumeProcess::run(){

    const int& max_abs_shift=m_maximumShift;
    const int& wnd_len=m_windowLength;

    Grid3DBounds bounds=m_volume->bounds();

    std::shared_ptr<Grid3D<float>> gmean( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> ggauss( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gmin( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gmax( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gpos( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gneg( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gangle( new Grid3D<float>(bounds) );
    std::shared_ptr<Grid3D<float>> gazimuth( new Grid3D<float>(bounds) );


    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1()+1; i<=bounds.inline2()-1; i++){

        for( int j=bounds.crossline1()+1; j<=bounds.crossline2()-1; j++){

            #pragma omp parallel for
            for( int k=wnd_len/2+1; k<bounds.sampleCount()-wnd_len/2-1; k++){

                int s1 = optimum_shift( *m_volume, i-1, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );
                int s2 = optimum_shift( *m_volume, i-1, j, i, j, k-wnd_len/2, wnd_len, max_abs_shift );
                int s3 = optimum_shift( *m_volume, i-1, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );

                int s4 = optimum_shift( *m_volume, i, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );
                int s5 = 0;
                int s6 = optimum_shift( *m_volume, i, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );

                int s7 = optimum_shift( *m_volume, i+1, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );
                int s8 = optimum_shift( *m_volume, i+1, j, i, j, k-wnd_len/2, wnd_len, max_abs_shift );
                int s9 = optimum_shift( *m_volume, i+1, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift );

                float Z1=1000.*bounds.sampleToTime(k+s1);
                float Z2=1000.*bounds.sampleToTime(k+s2);
                float Z3=1000.*bounds.sampleToTime(k+s3);

                float Z4=1000.*bounds.sampleToTime(k+s4);
                float Z5=1000.*bounds.sampleToTime(k);
                float Z6=1000.*bounds.sampleToTime(k+s6);

                float Z7=1000.*bounds.sampleToTime(k+s7);
                float Z8=1000.*bounds.sampleToTime(k+s8);
                float Z9=1000.*bounds.sampleToTime(k+s9);

                float a = (Z1+Z3+Z4+Z6+Z7+Z9)/12 - (Z2+Z5+Z8)/6;
                float b = (Z1+Z2+Z3+Z7+Z8+Z9)/12 - (Z4+Z5+Z6)/6;
                float c = (Z3+Z7-Z1-Z9)/4;
                float d = (Z3+Z6+Z9-Z1-Z4-Z7)/6;
                float e = (Z1+Z2+Z3-Z7-Z8-Z9)/6;
                float f = ( 2*(Z2+Z4+Z6+Z8) - (Z1+Z3+Z7+Z9) +5*Z5) / 9;

                float kmean  = ( a*(1+e*e) -c*d*e + b*(1+d*d) ) / std::pow( 1 + d*d + e*e, 1.5);
                float kgauss = ( 4*a*b - c*c) / std::pow( 1+d*d+e*e, 2);
                float kmax = kmean + sqrt( kmean*kmean - kgauss );
                float kmin = kmean - sqrt( kmean*kmean - kgauss );
                float kpos = a+b+sqrt( std::pow(a-b, 2) + c*c );
                float kneg = a+b-sqrt( std::pow(a-b, 2) + c*c );
                float kangle = std::atan( std::sqrt( d*d + e*e ) );
                float kazimuth = std::atan2( d, e);

                (*gmean)(i,j,k)=kmean;
                (*ggauss)(i,j,k)=kgauss;
                (*gmax)(i,j,k)=kmax;
                (*gmin)(i,j,k)=kmin;//kmin;
                (*gpos)(i,j,k)=kpos;
                (*gneg)(i,j,k)=kneg;
                (*gangle)(i,j,k)=kangle;
                (*gazimuth)(i,j,k)=kazimuth;
            }
        }

        if( (i-bounds.inline1()) % onePercent==0 ){
            emit progress((i-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volumes");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( m_output_attributes.contains(MIN_CURVATURE_STR)){
        QString name=QString("%1-minimum").arg(m_baseName);
        if( !project()->addVolume( name, gmin ) ) {
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_CURVATURE_STR)){
        QString name=QString("%1-maximum").arg(m_baseName);
        if( !project()->addVolume( name, gmax ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MEAN_CURVATURE_STR)){
        QString name=QString("%1-mean").arg(m_baseName);
        if( !project()->addVolume( name, gmean ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(GAUSS_CURVATURE_STR)){
        QString name=QString("%1-gaussian").arg(m_baseName);
        if( !project()->addVolume( name, ggauss ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_POS_CURVATURE_STR)){
        QString name=QString("%1-maximum-positive").arg(m_baseName);
        if( !project()->addVolume( name, gpos ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(MAX_NEG_CURVATURE_STR)){
        QString name=QString("%1-maximum-negative").arg(m_baseName);
        if( !project()->addVolume( name, gneg ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(DIP_ANGLE_STR)){
        QString name=QString("%1-dip-angle").arg(m_baseName);
        if( !project()->addVolume( name, gangle ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( m_output_attributes.contains(DIP_AZIMUTH_STR)){
        QString name=QString("%1-dip-azimuth").arg(m_baseName);
        if( !project()->addVolume( name, gazimuth ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
