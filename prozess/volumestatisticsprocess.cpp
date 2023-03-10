#include "volumestatisticsprocess.h"

#include<grid2d.h>
#include<memory>
#include<iostream>
#include<cmath>


#include "utilities.h"
//#include <Variance.h>

#include<QApplication>

namespace stats{

float min(float* p, size_t n){

    auto it = std::min_element( p, p+n-1 );
    return *it;
}

float max(float* p, size_t n){

    auto it = std::max_element( p, p+n-1 );
    return *it;
}

float maxABS(float* p, size_t n){

    float max=0;
    for(auto it=p; it!=p+n; it++){
        if(std::abs(*it)>max)
            max=std::abs(*it);
    }
    return max;
}


float mean(float* p, size_t n){

    double oldM, newM;

    for( auto it=p; it!=p+n; it++){

        if( it==p ){
            oldM=newM=*it;
        }
        else{
            newM = oldM + (*it - oldM)/(it-p);
        }
    }

    return newM;
}


float trimmed_mean(float* pfirst, size_t n){

    //std::vector<float> buf;
    //buf.reserve(n);
    auto plast=pfirst+n-1;
    std::sort(pfirst, plast);

    double keep=0.8;
    int start=0.5*(1.0-keep)*n;
    int stop=n-start;  // symmetric
    double sum=std::accumulate( pfirst+start, pfirst+stop, 0);
    auto nn=stop-start+1;

    return sum/nn;
}


float median(float* p, size_t n){

    std::nth_element( p, p+n/2, p+n-1 );    // resorts the buffer!!!
    return *(p+n/2);
}

float variance(float* p, size_t n){

    double oldM, newM;
    double oldS, newS;

    for( auto it=p; it!=p+n; it++){

        if( it==p){
            oldM = newM = *it;
            newS=oldS=0;
        }
        else{
            newM = oldM + (*it - oldM)/(it - p + 1);
            newS = oldS + (*it - oldM)*(*it - newM);
            oldM = newM;
            oldS = newS;
        }
    }

    return ( (n > 1) ? newS/(n - 1) : 0.0 );
}

float standardDeviation(float* p, size_t n){

    return std::sqrt(std::fabs(variance(p,n)));
}

float rms(float* p, size_t n){

    qreal sum2=0;
    for(auto it=p; it!=p+n; it++){
        sum2+=std::pow(static_cast<qreal>(*it),2);
    }
    return (n>0) ? static_cast<float>(std::sqrt(sum2/n)) : 0;
}

}

VolumeStatisticsProcess::VolumeStatisticsProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Statistics"), project, parent){

}

ProjectProcess::ResultCode VolumeStatisticsProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputName;
    QString statistic;
    QString ildipname;
    QString xldipname;
    QString topName;
    QString bottomName;
    try{
        statistic=getParam(parameters,"statistic");
        m_halfIlines=getParam(parameters,"half-ilines").toUInt();
        m_halfXlines=getParam(parameters,"half-xlines").toUInt();
        m_halfSamples=getParam(parameters,"half-samples").toUInt();
        inputName=getParam(parameters, "input-volume");
        m_volumeName=getParam(parameters, "output-volume");
        m_track=static_cast<bool>(getParam( parameters, "track-dips").toInt() );
        ildipname=getParam( parameters, "iline-dip");
        xldipname=getParam( parameters, "xline-dip");
        topName=getParam( parameters, "top-horizon");
        bottomName=getParam( parameters, "bottom-horizon");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_inputVolume=project()->loadVolume( inputName );
    if( !m_inputVolume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }
    m_bounds=m_inputVolume->bounds();

    if(topName!="NONE"){
        m_topHorizon=project()->loadGrid(GridType::Horizon, topName);
        if(!m_topHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(topName));
            return ResultCode::Error;
        }
    }

    if(bottomName!="NONE"){
        m_bottomHorizon=project()->loadGrid(GridType::Horizon, bottomName);
        if(!m_bottomHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(bottomName));
            return ResultCode::Error;
        }
    }

    if( m_track ){
        auto ildip=project()->loadVolume(ildipname);
        if( !ildip){
            setErrorString(tr("Loading volume \"%1\" failed!").arg(ildipname));
            return ResultCode::Error;
        }
        auto xldip=project()->loadVolume(xldipname);
        if( !xldip){
            setErrorString(tr("Loading volume \"%1\" failed!").arg(xldipname));
            return ResultCode::Error;
        }
        if( ildip->bounds()!=xldip->bounds()){
            setErrorString("Dip volumes geometries don't match!");
            return ResultCode::Error;
        }
        m_tracker = std::make_shared<VolumeDipTracker>( ildip, xldip);
    }


    m_volume=std::shared_ptr<Volume>(new Volume(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    if( statistic=="Minimum"){
        m_func=stats::min;
    }
    else if( statistic=="Maximum"){
        m_func=stats::max;
    }
    else if( statistic=="Maximum Absolute"){
        m_func=stats::maxABS;
    }
    else if( statistic=="Mean"){
        m_func=stats::mean;
    }
    else if( statistic=="Trimmed Mean"){
        m_func=stats::trimmed_mean;
    }
    else if( statistic=="Median"){
        m_func=stats::median;
    }
    else if( statistic=="Variance"){
        m_func=stats::variance;
    }
    else if( statistic=="Standard Deviation"){
        m_func=stats::standardDeviation;
    }
    else if( statistic=="RMS"){
        m_func=stats::rms;
    }
    else{
        setErrorString( "Unsupported statistic type!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}




/*
double variance( const Volume& vol, const std::vector<std::pair<int,int>>& locations, int hw, int k){

    double oldM, newM, oldS=0., newS;
    int n=0;

    int k1((k>=hw)?k - hw : 0);
    int k2((k+hw<vol.bounds().sampleCount()) ? k+hw : vol.bounds().sampleCount()-1);

    for( auto loc : locations ){

        const float* sam=&vol(loc.first, loc.second, 0);

        for( auto t=k1; t<=k2; t++){

            auto x = sam[t];
            if( x==vol.NULL_VALUE) continue;

            n++;

            if( n==1){
                oldM = newM = x;
                newS=oldS=0;
            }
            else{
                newM = oldM + (x - oldM)/n;
                newS = oldS + (x - oldM)*(x - newM);
                oldM = newM;
                oldS = newS;
            }
        }
    }

    //OLD
    return ( (n > 1) ? newS/(n - 1) : 0.0 );

    // NEW MAKE RELATIVE, RELATE TO MEAN, ton good yet!
    // return ( (n > 1) ?  ( newS/(n - 1) ) / (newM*newM) : 0.0 );
}
*/


void VolumeStatisticsProcess::fillBuffer( std::vector<float>& buffer, int i, int j, int k){

    for( int ii=i-m_halfIlines; ii<=i+m_halfIlines; ii++){
        for( int jj=j-m_halfXlines; jj<=j+m_halfXlines; jj++){

            if( !m_bounds.isInside(ii,jj)) continue;
            int k0=std::max(0,k-m_halfSamples);
            if(m_topHorizon){
                auto tmin=m_topHorizon->valueAt(ii,jj);
                if(tmin==m_topHorizon->NULL_VALUE) continue;
                k0=std::max( k0,
                             static_cast<int>( std::round( ( 0.001*tmin -m_bounds.ft() ) / m_bounds.dt() ) ) );
            }
            int k1=std::min(m_bounds.nt()-1,k+m_halfSamples);
            if(m_bottomHorizon){
                auto tmax=m_bottomHorizon->valueAt(ii,jj);
                if(tmax==m_bottomHorizon->NULL_VALUE) continue;
                k1=std::min( k1,
                             static_cast<int>( std::round( ( 0.001*tmax -m_bounds.ft() ) / m_bounds.dt() ) ) );
            }
            for( int kk=k0; kk<=k1; kk++){
                auto sam = (*m_inputVolume)(ii,jj,kk);
                if(sam!=m_inputVolume->NULL_VALUE) buffer.push_back(sam);
            }
            if(ii==701 && jj==1539){
            std::cout<<"ii="<<ii<<" jj="<<jj<<" k="<<k<<" k0="<<k0<<" k1="<<k1<<" n="<<buffer.size()<<std::endl<<std::flush;
            }
        }
    }
}


void VolumeStatisticsProcess::fillBuffer_tracked( std::vector<float>& buffer, int i, int j, int k){


    for( int ii=i-m_halfIlines; ii<=i+m_halfIlines; ii++){

        for( int jj=j-m_halfXlines; jj<=j+m_halfXlines; jj++){

            if( !m_bounds.isInside(ii,jj)) continue;

            auto t=m_tracker->track( i, j, m_bounds.sampleToTime(k),  ii, jj);
            auto sam = m_inputVolume->value(ii, jj, t);
            if(sam!=m_inputVolume->NULL_VALUE) buffer.push_back(sam);

        }
    }
}


ProjectProcess::ResultCode VolumeStatisticsProcess::run(){

    auto bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    // why skip first and last lines? 2d is handled as just 1 inline, skipping first/last wont work!
    //for( int i=bounds.i1()+1; i<=bounds.i2()-1; i++){
        //for( int j=bounds.j1()+1; j<=bounds.j2()-1; j++){

    for( int i=bounds.i1(); i<=bounds.i2(); i++){
        for( int j=bounds.j1(); j<=bounds.j2(); j++){
            int k0=0;
            if(m_topHorizon){
                auto tmin=m_topHorizon->valueAt(i,j);
                if(tmin==m_topHorizon->NULL_VALUE) continue;
                k0=std::max( k0,
                             static_cast<int>( std::round( ( 0.001*tmin -m_bounds.ft() ) / m_bounds.dt() ) ) );
            }
            int k1=m_bounds.nt();
            if(m_bottomHorizon){
                auto tmax=m_bottomHorizon->valueAt(i,j);
                if(tmax==m_bottomHorizon->NULL_VALUE) continue;
                k1=std::min( k1,
                             static_cast<int>( std::round( ( 0.001*tmax -m_bounds.ft() ) / m_bounds.dt() ) ) );
            }
            std::cout<<"!!! k0="<<k0<<" k1="<<k1<<std::endl<<std::flush;
            //#pragma omp parallel for
            for( int k=k0; k<k1; k++){

                std::vector<float> buffer;

                /*
                for( int ii=i-m_halfIlines; ii<=i+m_halfIlines; ii++){
                    for( int jj=j-m_halfXlines; jj<=j+m_halfXlines; jj++){
                        if( !bounds.isInside(ii,jj)) continue;
                        for( int kk=std::max(0, k-m_halfSamples); kk<=std::min(bounds.nt()-1, k+m_halfSamples); kk++){
                            auto sam = (*m_inputVolume)(ii,jj,kk);
                            if(sam!=m_inputVolume->NULL_VALUE) buffer.push_back(sam);
                        }
                    }
                }
                */

                if( m_track){
                    fillBuffer_tracked(buffer, i, j, k);
                }
                else{
                    fillBuffer(buffer, i, j, k);
                }

                if( buffer.empty()) continue;

                (*m_volume)(i,j,k)=m_func(&buffer[0], buffer.size() );
            }
/*
            #pragma omp parallel for
            for( int k=0; k<bounds.sampleCount(); k++){
                auto x=variance(*m_inputVolume, locations, m_halfSamples, k);
                //if( !std::isnan(x) )
                (*m_volume)(i,j,k)=x;
            }
*/
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addVolume( m_volumeName, m_volume, params() ) ) {
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    return ResultCode::Ok;
}


