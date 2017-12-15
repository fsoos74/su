#include "variancevolumeprocess.h"

#include<grid2d.h>
#include<memory>
#include<iostream>



#include "utilities.h"
//#include <Variance.h>

#include<QApplication>

VarianceVolumeProcess::VarianceVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Variance Volume"), project, parent){

}

ProjectProcess::ResultCode VarianceVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("half-ilines"))){
        setErrorString("Parameters contain no half-ilines");
        return ResultCode::Error;
    }
    m_halfIlines=parameters.value(QString("half-ilines") ).toUInt();

    if( !parameters.contains(QString("half-xlines"))){
        setErrorString("Parameters contain no half-xlines");
        return ResultCode::Error;
    }
    m_halfXlines=parameters.value(QString("half-xlines") ).toUInt();

    if( !parameters.contains(QString("half-samples"))){
        setErrorString("Parameters contain no half-samples");
        return ResultCode::Error;
    }
    m_halfSamples=parameters.value(QString("half-samples") ).toUInt();

    if( !parameters.contains(QString("input-volume"))){

        setErrorString("Parameters contain no input-volume!");
        return ResultCode::Error;
    }
    QString inputName=parameters.value( QString("input-volume") );
    m_inputVolume=project()->loadVolume( inputName );
    if( !m_inputVolume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }
    m_bounds=m_inputVolume->bounds();

    if( !parameters.contains(QString("output-volume"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("output-volume"));

    m_volume=std::shared_ptr<Volume>(new Volume(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    setParams( parameters );

    return ResultCode::Ok;
}

double variance( const Volume& vol, const std::vector<std::pair<int,int>>& locations, int hw, int k){

    double oldM, newM, oldS=0., newS;
    int n=0;

    int k1((k>=hw)?k - hw : 0);
    int k2((k+hw<vol.bounds().nt()) ? k+hw : vol.bounds().nt()-1);

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

ProjectProcess::ResultCode VarianceVolumeProcess::run(){

    auto bounds=m_volume->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1()+1; i<=bounds.i2()-1; i++){

        for( int j=bounds.j1()+1; j<=bounds.j2()-1; j++){

            std::vector<std::pair<int,int>> locations;
            for( int ii=i-m_halfIlines; ii<=i+m_halfIlines; ii++){
                for( int jj=j-m_halfXlines; jj<=j+m_halfXlines; jj++){
                    if( bounds.isInside(ii,jj)){
                        locations.push_back( std::make_pair(ii,jj) );
                    }
                }
            }

            #pragma omp parallel for
            for( int k=0; k<bounds.nt(); k++){
                auto x=variance(*m_inputVolume, locations, m_halfSamples, k);
                //if( !std::isnan(x) )
                (*m_volume)(i,j,k)=x;
            }

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
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    return ResultCode::Ok;
}


