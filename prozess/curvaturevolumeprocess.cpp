#include "curvaturevolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include <curvature_attributes.h> // names
#include<iostream>
#include<cmath>
#include<omp.h>


CurvatureVolumeProcess::CurvatureVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Curvature Volume"), project, parent){

}

ProjectProcess::ResultCode CurvatureVolumeProcess::init( const QMap<QString, QString>& parameters ){

    //return ResultCode::Ok;
    setParams(parameters);

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

namespace{
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
int optimum_shift( const Volume& vol, int i1, int j1, int i2, int j2,
            int wnd_start, int wnd_len=5, int max_abs_shift=5 ){

    auto trc1 = &vol(i1, j1, 0);
    auto trc2 = &vol(i2, j2, 0);

    if( wnd_start - max_abs_shift < 0 ) return 0;
    if( wnd_start + wnd_len + max_abs_shift >=vol.bounds().nt()) return 0;

    auto best_shift=0;
    auto best_cor = std::numeric_limits<double>::lowest();
    for( auto shift = -max_abs_shift; shift<=max_abs_shift; shift++){
        auto cor = corr( &trc1[wnd_start], &trc1[wnd_start+wnd_len],
                &trc2[wnd_start+shift] );
        if( cor>best_cor){
            best_cor=cor;
            best_shift=shift;
        }
    }

    return best_shift;
}
}

ProjectProcess::ResultCode CurvatureVolumeProcess::run(){

    //return ResultCode::Ok;
    const int& max_abs_shift=m_maximumShift;
    const int& wnd_len=m_windowLength;

    Grid3DBounds bounds=m_volume->bounds();

    std::shared_ptr<Volume> gmean;
    std::shared_ptr<Volume> ggauss;
    std::shared_ptr<Volume> gmin;
    std::shared_ptr<Volume> gmax;
    std::shared_ptr<Volume> gpos;
    std::shared_ptr<Volume> gneg;
    std::shared_ptr<Volume> gangle;
    std::shared_ptr<Volume> gazimuth;

    try{
    if( m_output_attributes.contains(MIN_CURVATURE_STR)) gmin=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(MAX_CURVATURE_STR)) gmax=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(MEAN_CURVATURE_STR)) gmean=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(GAUSS_CURVATURE_STR)) ggauss=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(MAX_POS_CURVATURE_STR)) gpos=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(MAX_NEG_CURVATURE_STR)) gneg=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(DIP_ANGLE_STR)) gangle=std::make_shared<Volume>(bounds);
    if( m_output_attributes.contains(DIP_AZIMUTH_STR)) gazimuth=std::make_shared<Volume>(bounds);
    }
    catch(...){
        gmean.reset();
        ggauss.reset();
        gmin.reset();
        gmax.reset();
        gpos.reset();
        gneg.reset();
        gangle.reset();
        gazimuth.reset();
        setErrorString("Error allocating attribute volumes!");
        return ResultCode::Error;
    }

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1()+1; i<bounds.i2()-1; i++){

        for( int j=bounds.j1()+1; j<bounds.j2()-1; j++){

            #pragma omp parallel for
            for( int k=wnd_len/2+1; k<bounds.nt()-wnd_len/2-1; k++){
                //std::cout<<"k="<<k<<std::endl<<std::flush;
                int s1 = (i>bounds.i1()) ?
                            optimum_shift( *m_volume, i-1, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) : 0;
                int s2 =(i>bounds.i1()) ?
                            optimum_shift( *m_volume, i-1, j, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) : 0;
                int s3 = (i>bounds.i1() && j<bounds.j2()) ?
                            optimum_shift( *m_volume, i-1, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) : 0;

                int s4 = (j>bounds.j1()) ?
                            optimum_shift( *m_volume, i, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) :0;
                int s5 = 0;
                int s6 = ( j<bounds.j2()) ?
                            optimum_shift( *m_volume, i, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) :0;

                int s7 = (i<bounds.i1() && j>bounds.j1()) ?
                            optimum_shift( *m_volume, i+1, j-1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) :0;
                int s8 = (i<bounds.i2()) ?
                            optimum_shift( *m_volume, i+1, j, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) : 0;
                int s9 = (i<bounds.i2() && j<bounds.j2()) ?
                            optimum_shift( *m_volume, i+1, j+1, i, j, k-wnd_len/2, wnd_len, max_abs_shift ) :0;

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

                if( gmean ) (*gmean)(i,j,k)=kmean;
                if( ggauss) (*ggauss)(i,j,k)=kgauss;
                if( gmax ) (*gmax)(i,j,k)=kmax;
                if( gmin ) (*gmin)(i,j,k)=kmin;//kmin;
                if( gpos ) (*gpos)(i,j,k)=kpos;
                if( gneg ) (*gneg)(i,j,k)=kneg;
                if( gangle ) (*gangle)(i,j,k)=kangle;
                if( gazimuth ) (*gazimuth)(i,j,k)=kazimuth;
            }
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volumes");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( gmin){
        QString name=QString("%1-minimum").arg(m_baseName);
        if( !project()->addVolume( name, gmin, params() ) ) {
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gmax ){
        QString name=QString("%1-maximum").arg(m_baseName);
        if( !project()->addVolume( name, gmax, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gmean ){
        QString name=QString("%1-mean").arg(m_baseName);
        if( !project()->addVolume( name, gmean, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( ggauss ){
        QString name=QString("%1-gaussian").arg(m_baseName);
        if( !project()->addVolume( name, ggauss, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gpos ){
        QString name=QString("%1-maximum-positive").arg(m_baseName);
        if( !project()->addVolume( name, gpos, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gneg ){
        QString name=QString("%1-maximum-negative").arg(m_baseName);
        if( !project()->addVolume( name, gneg, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gangle ){
        QString name=QString("%1-dip-angle").arg(m_baseName);
        if( !project()->addVolume( name, gangle, params() ) ){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(name) );
            return ResultCode::Error;
        }
    }

    if( gazimuth ){
        QString name=QString("%1-dip-azimuth").arg(m_baseName);
        if( !project()->addVolume( name, gazimuth, params() ) ){
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
