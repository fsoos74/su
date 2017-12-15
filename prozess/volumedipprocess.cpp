#include "volumedipprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include <semblance.h>

using namespace std::placeholders;

namespace{

QMap<VolumeDipProcess::Method, QString> method_name_lookup{
    {VolumeDipProcess::Method::DERIVATION, "derivation"},
    {VolumeDipProcess::Method::DERIVATION_5P, "5 point derivation"},
    {VolumeDipProcess::Method::DERIVATION_7P, "7 point derivation"},
    {VolumeDipProcess::Method::CORRELATION, "correlation"}
};

}

QString VolumeDipProcess::toQString(Method op){
    return method_name_lookup.value(op, method_name_lookup.value(Method::DERIVATION));
}

VolumeDipProcess::Method VolumeDipProcess::toMethod(QString name){
    return method_name_lookup.key(name, Method::DERIVATION);
}

QStringList VolumeDipProcess::methodList(){
    return method_name_lookup.values();
}



VolumeDipProcess::VolumeDipProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Dip"), project, parent){

}

ProjectProcess::ResultCode VolumeDipProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString iname;
    QString methodName;

    try{
        m_ilineDipName=getParam(parameters, "iline-dip");
        m_xlineDipName=getParam(parameters, "xline-dip");
        iname=getParam(parameters, "input-volume");
        methodName=getParam(parameters, "method");
        m_window_samples=getParam(parameters, "window-samples").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    auto method=toMethod(methodName);
    switch( method ){
        case Method::DERIVATION: m_func=std::bind( &VolumeDipProcess::dips_derivative, this, _1, _2, _3); m_margin=1; break;
        case Method::DERIVATION_5P: m_func=std::bind( &VolumeDipProcess::dips_derivative_5p, this, _1, _2, _3); m_margin=2; break;
        case Method::DERIVATION_7P: m_func=std::bind( &VolumeDipProcess::dips_derivative_7p, this, _1, _2, _3); m_margin=3; break;
        case Method::CORRELATION: m_func=std::bind( &VolumeDipProcess::dips_correlation, this, _1, _2, _3); m_margin=1; break;
    }

    emit currentTask("Loading Volume");
    emit started(1);
    qApp->processEvents();
    m_inputVolume=project()->loadVolume(iname);
    if( !m_inputVolume){
        setErrorString(QString("Loading input volume failed!"));
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    m_ilineDipVolume=std::make_shared<Volume>(m_inputVolume->bounds(), m_inputVolume->domain(), VolumeType::Other, 0);
    if( !m_ilineDipVolume){
        setErrorString("Allocating iline dip volume failed!");
        return ResultCode::Error;
    }

    m_xlineDipVolume=std::make_shared<Volume>(m_inputVolume->bounds(), m_inputVolume->domain(), VolumeType::Other, 0 );
    if( !m_xlineDipVolume){
        setErrorString("Allocating xline dip volume failed!");
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
int optimum_shift( const Volume& vol, int i1, int j1, int i2, int j2,
            int wnd_start, int wnd_len=5, int max_abs_shift=5 ){

    auto trc1 = &vol(i1, j1, 0);
    auto trc2 = &vol(i2, j2, 0);

    int min_shift = std::max( -wnd_start, -max_abs_shift );
    int max_shift = std::min( max_abs_shift, vol.bounds().nt() - wnd_start - wnd_len -1 );
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

ProjectProcess::ResultCode VolumeDipProcess::run(){

    Grid3DBounds bounds=m_inputVolume->bounds();

    emit currentTask("Computing output volumes");
    emit started(bounds.ni() - 2);
    qApp->processEvents();

    const int Q=m_margin;
    const float EPS=0.00001;

    for( int i=bounds.i1()+Q; i<=bounds.i2()-Q; i++){

        for( int j=bounds.j1()+Q; j<bounds.j2()-Q; j++){

            #pragma omp parallel for
            for( int k=Q; k<bounds.nt()-Q; k++){

                /* derivative based
                 *
                auto v211=(*m_inputVolume)(i+1,j,k);
                if( v211==m_inputVolume->NULL_VALUE) continue;
                auto v011=(*m_inputVolume)(i-1,j,k);
                if( v011==m_inputVolume->NULL_VALUE) continue;
                double di = ( v211 - v011)/(2*1);

                auto v121=(*m_inputVolume)(i,j+1,k);
                if( v121==m_inputVolume->NULL_VALUE) continue;
                auto v101=(*m_inputVolume)(i,j-1,k);
                if( v101==m_inputVolume->NULL_VALUE) continue;
                double dj = ( v121 - v101)/(2*1);

                auto v112=(*m_inputVolume)(i,j,k+1);
                if( v112==m_inputVolume->NULL_VALUE) continue;
                auto v110=(*m_inputVolume)(i,j,k-1);
                if( v110==m_inputVolume->NULL_VALUE) continue;
                double dk = ( v112 - v110)/(2*1);

                if( std::fabs(dk) < std::numeric_limits<float>::epsilon()) continue;

                auto ildip = -di/dk;
                (*m_ilineDipVolume)(i,j,k)= ildip;

                auto xldip = -dj/dk;
                (*m_xlineDipVolume)(i,j,k)= xldip;

                */

                /*
                // derivative based 5-point

                auto fk1=(*m_inputVolume)(i,j,k+2);
                auto fk2=(*m_inputVolume)(i,j,k+1);
                auto fk4=(*m_inputVolume)(i,j,k-1);
                auto fk5=(*m_inputVolume)(i,j,k-2);
                if( fk1==m_inputVolume->NULL_VALUE || fk2==m_inputVolume->NULL_VALUE ||
                        fk4==m_inputVolume->NULL_VALUE || fk5==m_inputVolume->NULL_VALUE) continue;
                double dk=(-fk1 + 8*fk2 - 8*fk4 + fk5)/12;

                auto fi1=(*m_inputVolume)(i+2,j,k);
                auto fi2=(*m_inputVolume)(i+1,j,k);
                auto fi4=(*m_inputVolume)(i-1,j,k);
                auto fi5=(*m_inputVolume)(i-2,j,k);
                if( fi1==m_inputVolume->NULL_VALUE || fi2==m_inputVolume->NULL_VALUE ||
                        fi4==m_inputVolume->NULL_VALUE || fi5==m_inputVolume->NULL_VALUE) continue;
                double di=(-fi1 + 8*fi2 - 8*fi4 + fi5)/12;

                auto fx1=(*m_inputVolume)(i,j+2,k);
                auto fx2=(*m_inputVolume)(i,j+1,k);
                auto fx4=(*m_inputVolume)(i,j-1,k);
                auto fx5=(*m_inputVolume)(i,j-2,k);
                if( fx1==m_inputVolume->NULL_VALUE || fx2==m_inputVolume->NULL_VALUE ||
                        fx4==m_inputVolume->NULL_VALUE || fx5==m_inputVolume->NULL_VALUE) continue;
                double di=(-fi1 + 8*fi2 - 8*fi4 + fi5)/12;

                auto ildip=-di/(EPS + dk);
                (*m_ilineDipVolume)(i,j,k)= ildip;
                */
                /*
                // correlation based

                auto di1=optimum_shift( *m_inputVolume, i-1, j, i, j, k-2, 5, 5);
                auto di2=optimum_shift( *m_inputVolume, i, j, i+1, j, k-2, 5, 5);
                (*m_ilineDipVolume)(i,j,k)= 0.5*(di1+di2);

                auto dx1=optimum_shift( *m_inputVolume, i, j-1, i, j, k-2, 5, 5);
                auto dx2=optimum_shift( *m_inputVolume, i, j, i, j+1, k-2, 5, 5);
                (*m_xlineDipVolume)(i,j,k)=0.5*(dx1+dx2);
                */

                auto dips=m_func(i,j,k);
                (*m_ilineDipVolume)(i,j,k)=dips.first;
                (*m_xlineDipVolume)(i,j,k)=dips.second;
            }
        }

        emit progress( i-bounds.i1() -1 );
        qApp->processEvents();

    }

    emit currentTask("Saving output volumes");
    emit started(2);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_ilineDipName, m_ilineDipVolume,  params() )){
        setErrorString( QString("Could not save volume \"%1\"!").arg(m_ilineDipName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    if( !project()->addVolume( m_xlineDipName, m_xlineDipVolume,  params() )){
        setErrorString( QString("Could not save volume \"%1\"!").arg(m_xlineDipName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}


std::pair<float, float> VolumeDipProcess::dips_derivative( int i, int j, int k){

    // numerical derivative

    const float EPS=0.000001;

    auto v211=(*m_inputVolume)(i+1,j,k);
    auto v011=(*m_inputVolume)(i-1,j,k);
    if( v011==m_inputVolume->NULL_VALUE || v211==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float di = ( v211 - v011)/(2*1);

    auto v121=(*m_inputVolume)(i,j+1,k);
    auto v101=(*m_inputVolume)(i,j-1,k);
    if( v121==m_inputVolume->NULL_VALUE || v101==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float dj = ( v121 - v101)/(2*1);

    auto v112=(*m_inputVolume)(i,j,k+1);
    auto v110=(*m_inputVolume)(i,j,k-1);
    if( v112==m_inputVolume->NULL_VALUE || v110==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float dk = ( v112 - v110)/(2*1);

    if( std::fabs(dk) <EPS ) return std::make_pair( 0.f, 0.f );

    auto ildip = -di/dk;
    auto xldip = -dj/dk;
    return std::make_pair( ildip, xldip );
}

std::pair<float, float> VolumeDipProcess::dips_derivative_5p( int i, int j, int k){

    // derivative based 5-point

    const float EPS=0.000001;

    auto fk1=(*m_inputVolume)(i,j,k+2);
    auto fk2=(*m_inputVolume)(i,j,k+1);
    auto fk4=(*m_inputVolume)(i,j,k-1);
    auto fk5=(*m_inputVolume)(i,j,k-2);
    if( fk1==m_inputVolume->NULL_VALUE || fk2==m_inputVolume->NULL_VALUE ||
            fk4==m_inputVolume->NULL_VALUE || fk5==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float dk=(-fk1 + 8*fk2 - 8*fk4 + fk5)/12;

    auto fi1=(*m_inputVolume)(i+2,j,k);
    auto fi2=(*m_inputVolume)(i+1,j,k);
    auto fi4=(*m_inputVolume)(i-1,j,k);
    auto fi5=(*m_inputVolume)(i-2,j,k);
    if( fi1==m_inputVolume->NULL_VALUE || fi2==m_inputVolume->NULL_VALUE ||
            fi4==m_inputVolume->NULL_VALUE || fi5==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float di=(-fi1 + 8*fi2 - 8*fi4 + fi5)/12;

    auto fx1=(*m_inputVolume)(i,j+2,k);
    auto fx2=(*m_inputVolume)(i,j+1,k);
    auto fx4=(*m_inputVolume)(i,j-1,k);
    auto fx5=(*m_inputVolume)(i,j-2,k);
    if( fx1==m_inputVolume->NULL_VALUE || fx2==m_inputVolume->NULL_VALUE ||
            fx4==m_inputVolume->NULL_VALUE || fx5==m_inputVolume->NULL_VALUE) return std::make_pair( 0.f, 0.f );
    float dx=(-fx1 + 8*fx2 - 8*fx4 + fx5)/12;

    if( std::fabs(dk) <EPS ) return std::make_pair( 0.f, 0.f );

    float ildip=-di/dk;
    float xldip=-dx/dk;
    return std::make_pair( ildip, xldip );
}


std::pair<float, float> VolumeDipProcess::dips_derivative_7p( int i, int j, int k){

    // derivative based 5-point

    const float EPS=0.000001;

    auto fk1=(*m_inputVolume)(i,j,k-3);
    auto fk2=(*m_inputVolume)(i,j,k-2);
    auto fk3=(*m_inputVolume)(i,j,k-1);
    auto fk5=(*m_inputVolume)(i,j,k+1);
    auto fk6=(*m_inputVolume)(i,j,k+2);
    auto fk7=(*m_inputVolume)(i,j,k+3);
    if( fk1==m_inputVolume->NULL_VALUE || fk2==m_inputVolume->NULL_VALUE || fk3==m_inputVolume->NULL_VALUE ||
            fk5==m_inputVolume->NULL_VALUE || fk6==m_inputVolume->NULL_VALUE || fk7==m_inputVolume->NULL_VALUE ) return std::make_pair( 0.f, 0.f );
    float dk=(-fk1 + 9*fk2 -45*fk3 + 45*fk5 -9*fk6 + fk7)/60;

    auto fi1=(*m_inputVolume)(i-3,j,k);
    auto fi2=(*m_inputVolume)(i-2,j,k);
    auto fi3=(*m_inputVolume)(i-1,j,k);
    auto fi5=(*m_inputVolume)(i+1,j,k);
    auto fi6=(*m_inputVolume)(i+2,j,k);
    auto fi7=(*m_inputVolume)(i+3,j,k);
    if( fi1==m_inputVolume->NULL_VALUE || fi2==m_inputVolume->NULL_VALUE || fi3==m_inputVolume->NULL_VALUE ||
            fi5==m_inputVolume->NULL_VALUE || fi6==m_inputVolume->NULL_VALUE || fi7==m_inputVolume->NULL_VALUE ) return std::make_pair( 0.f, 0.f );
    float di=(-fi1 + 9*fi2 -45*fi3 + 45*fi5 -9*fi6 + fi7)/60;

    auto fx1=(*m_inputVolume)(i,j-3,k);
    auto fx2=(*m_inputVolume)(i,j-2,k);
    auto fx3=(*m_inputVolume)(i,j-1,k);
    auto fx5=(*m_inputVolume)(i,j+1,k);
    auto fx6=(*m_inputVolume)(i,j+2,k);
    auto fx7=(*m_inputVolume)(i,j+3,k);
    if( fx1==m_inputVolume->NULL_VALUE || fx2==m_inputVolume->NULL_VALUE || fx3==m_inputVolume->NULL_VALUE ||
            fx5==m_inputVolume->NULL_VALUE || fx6==m_inputVolume->NULL_VALUE || fx7==m_inputVolume->NULL_VALUE ) return std::make_pair( 0.f, 0.f );
    float dx=(-fx1 + 9*fx2 -45*fx3 + 45*fx5 -9*fx6 + fx7)/60;

    if( std::fabs(dk) <EPS ) return std::make_pair( 0.f, 0.f );

    float ildip=-di/dk;
    float xldip=-dx/dk;
    return std::make_pair( ildip, xldip );
}


std::pair<float, float> VolumeDipProcess::dips_correlation( int i, int j, int k){

    // correlation based

    const int W=m_window_samples;

    auto di1=optimum_shift( *m_inputVolume, i-1, j, i, j, k-W/2, W, W/2);
    auto di2=optimum_shift( *m_inputVolume, i, j, i+1, j, k-W/2, W, W/2);
    float ildip= 0.5*(di1+di2);

    auto dx1=optimum_shift( *m_inputVolume, i, j-1, i, j, k-W/2, W, W/2);
    auto dx2=optimum_shift( *m_inputVolume, i, j, i, j+1, k-W/2, W, W/2);
    float xldip=0.5*(dx1+dx2);

    return std::make_pair( ildip, xldip );
}


