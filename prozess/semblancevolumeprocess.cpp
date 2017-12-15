#include "semblancevolumeprocess.h"

#include<grid2d.h>
#include<memory>
#include<iostream>



#include "utilities.h"
#include <semblance.h>

#include<QApplication>

SemblanceVolumeProcess::SemblanceVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Semblance Volume"), project, parent){

}

ProjectProcess::ResultCode SemblanceVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputName;
    QString ildipname;
    QString xldipname;

    try{

        m_halfIlines=getParam( parameters, "half-ilines").toUInt();
        m_halfXlines=getParam(parameters, "half-xlines").toUInt();
        m_halfSamples=getParam( parameters, "half-samples").toUInt();
        inputName=getParam(parameters, "input-volume");
        m_volumeName=getParam(parameters, "output-volume");
        m_track=static_cast<bool>( getParam( parameters, "track-dips").toInt());
        ildipname=getParam( parameters, "iline-dip");
        xldipname=getParam( parameters, "xline-dip");
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

    m_bounds=m_inputVolume->bounds();
    m_volume=std::shared_ptr<Volume>(new Volume(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode SemblanceVolumeProcess::run(){

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

            if(!m_tracker){
            #pragma omp parallel for
            for( int k=0; k<bounds.nt(); k++){
                (*m_volume)(i,j,k)=semblance(*m_inputVolume, locations, m_halfSamples, k);
            }
            }

            else{
                for( int k=0; k<bounds.nt(); k++){
                    auto t0=m_inputVolume->bounds().sampleToTime(k);
                    std::vector<std::tuple<int,int,double>> path;
                    path.reserve(locations.size());
                    for( auto loc : locations){
                        path.push_back( std::make_tuple( loc.first, loc.second, m_tracker->track( i, j, t0,  loc.first, loc.second)));
                    }
                    (*m_volume)(i,j,k)=semblance(*m_inputVolume, path, m_halfSamples);
                }

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


