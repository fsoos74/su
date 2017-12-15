#include "buildwellvolumeprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>
#include <wellpath.h>ö
#include <omp.h>
#include<iostream>

BuildWellVolumeProcess::BuildWellVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Build Well Volume"), project, parent){

}

namespace{
Log medianFilter( const Log& log, int len ){

    Log res( "median filtered", log.unit(), "median filtered", log.z0(), log.dz(), log.nz() );

    for( int i=0; i<log.nz(); i++ ){

        int start = std::max( 0, i-len/2);
        int stop = std::min(log.nz()-1, i + len/2 );
        std::vector<double> buf;
        buf.reserve(len);
        for( int i=start; i<=stop; i++){
            if( log[i]==log.NULL_VALUE) continue;
            buf.push_back(log[i]);
        }

        if( buf.size()>0 ){
            std::nth_element( &buf[0], &buf[buf.size()/2], &buf[buf.size()-1] );
            res[i]=buf[buf.size()/2];
        }
        else{
            res[i]=res.NULL_VALUE;
        }
    }

    return res;
}
}

ProjectProcess::ResultCode BuildWellVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    int minIline;
    int maxIline;
    int minXline;
    int maxXline;
    int minZ;
    int maxZ;
    int dz;
    QString topName;
    QString bottomName;

    try{

        m_outputName=getParam(parameters, "volume");
        minIline=getParam(parameters, "min-iline").toInt();
        maxIline=getParam( parameters, "max-iline").toInt();
        minXline=getParam( parameters, "min-xline").toInt();
        maxXline=getParam( parameters, "max-xline").toInt();
        minZ=getParam( parameters, "min-z").toInt();
        maxZ=getParam( parameters, "max-z").toInt();
        dz=getParam( parameters, "dz").toInt();
        m_filterLen=getParam(parameters, "filter-len").toInt();
        topName=getParam( parameters, "top-horizon");
        bottomName=getParam( parameters, "bottom-horizon");

        m_logName=getParam( parameters, "log");
        int i=0;
        while(1){
           QString name=QString("well%1").arg(++i);
           if( parameters.contains(name)){
               auto well=parameters.value(name);
               m_wellNames.push_back(well);
           }
           else{
               break;
           }
        }
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    if( m_wellNames.empty()){
        setErrorString("No wells selected!");
        return ResultCode::Error;
    }

    emit currentTask("Loading Horizons");
    emit started(2);
    qApp->processEvents();
    if(topName!="NONE"){
        m_topHorizon=project()->loadGrid(GridType::Horizon, topName);
        if(!m_topHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(topName));
            return ResultCode::Error;
        }
    }
    emit progress(1);
    qApp->processEvents();
    qApp->processEvents();
    if(bottomName!="NONE"){
        m_bottomHorizon=project()->loadGrid(GridType::Horizon, bottomName);
        if(!m_bottomHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(bottomName));
            return ResultCode::Error;
        }
    }
    emit progress(2);
    qApp->processEvents();

    emit currentTask("Loading logs");
    emit started(m_wellNames.size());
    qApp->processEvents();
    for( int i=0; i<m_wellNames.size(); i++){

        auto well=m_wellNames[i];
        auto winfo = project()->getWellInfo(well);

        auto log= project()->loadLog(well, m_logName );

        if( m_filterLen>0){
            *log=medianFilter(*log, m_filterLen);
        }

        m_logs.push_back( log );

        auto path=project()->loadWellPath(well);
        m_paths.push_back(path);

        emit progress(i+1);
        qApp->processEvents();
    }

    int nz=1 + ( maxZ - minZ )/dz;
    Grid3DBounds bounds(minIline, maxIline, minXline, maxXline, nz, 0.001*minZ, 0.001*dz);

    m_volume=std::shared_ptr<Volume >( new Volume(bounds));

    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode BuildWellVolumeProcess::run(){

    const double eps=0.1;

    Grid3DBounds bounds=m_volume->bounds();


    emit currentTask("Computing output volume");
    emit started(bounds.nt());
    qApp->processEvents();

    QTransform ilxl_to_xy;
    QTransform xy_to_ilxl;
    project()->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( int k=0; k<bounds.nt(); k++){

        auto zo=1000*m_volume->bounds().sampleToTime(k); // convert to msec

        for( int i=bounds.i1(); i<=bounds.i2(); i++){

            for( int j=bounds.j1(); j<=bounds.j2(); j++){

                // find top and bottom depth at output loc in mills, horizons are in mills
                auto topo=(m_topHorizon)?m_topHorizon->valueAt(i,j) : 1000*m_volume->bounds().ft();
                if(topo==m_topHorizon->NULL_VALUE) continue;
                auto boto=(m_bottomHorizon)?m_bottomHorizon->valueAt(i,j) : 1000*m_volume->bounds().lt();
                if(boto==m_bottomHorizon->NULL_VALUE) continue;
                if(boto<=topo ) continue;
                if( zo<topo || zo>boto) continue;
                // compute relative depth at output location, horizons are in mills
                qreal relo=(zo-topo)/(boto-topo);

                QPointF opos = ilxl_to_xy.map(QPoint(i,j));
                qreal sum=0;
                qreal wsum=0;

                //THREAD SAFETY ISSUE, changes result #pragma omp parallel for
                for( int l=0; l<m_logs.size(); l++){
                    //auto wpos=location(*m_paths[l], -zo);
                    auto wpos=QPointF((*m_paths[l])[0].x(), (*m_paths[l])[0].y() );     // try vertical
                    qreal dx=wpos.x()-opos.x();
                    qreal dy=wpos.y()-opos.y();
                    qreal dist=std::sqrt( dx*dx + dy*dy );

                    qreal w=1./dist;  // handle dist==0!!!!

                    // find relative log depth, use ili xl need to change this to coords!!!
                    // assume that horizons are constant in vicinity of well otherwise horizon( path( zw ) ) but also zw( horizons)
                    auto ilxlw=xy_to_ilxl.map(wpos);//QPointF(wpos.x(), wpos.y()));
                    auto iw=ilxlw.x();
                    auto jw=ilxlw.y();
                    auto topw=(m_topHorizon)?m_topHorizon->valueAt(iw,jw) : 1000*m_volume->bounds().ft();
                    if(topw==m_topHorizon->NULL_VALUE) continue;
                    auto botw=(m_bottomHorizon)?m_bottomHorizon->valueAt(iw,jw) : 1000*m_volume->bounds().lt();
                    if(botw==m_bottomHorizon->NULL_VALUE) continue;
                    auto zw= - ( topw+relo*(botw-topw) );   // z-axis points upwards, horizons are in depth not z
                    auto zmd=m_paths[l]->mdAtZ(zw);
                    auto value=(*m_logs[l])(zmd);

                    //if( i==694 && j==1400 ){
                    //   std::cout<<"l="<<l<<" dist="<<dist<<" topw="<<topw<<" botw="<<botw<<"zw="<<zw<<" zmd="<<zmd<<" value="<<value<<std::endl<<std::flush;
                    //}

                    if( value==m_logs[l]->NULL_VALUE) continue;

                    sum+=w*value;
                    wsum+=w;
                }

                if( wsum ) (*m_volume)(i,j,k)= sum/wsum;//relo;
            }
        }

        emit progress(k);
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
