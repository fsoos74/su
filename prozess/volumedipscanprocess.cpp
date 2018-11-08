#include "volumedipscanprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include <semblance.h>
#include <volumeinterpolation.h>

VolumeDipScanProcess::VolumeDipScanProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Dip"), project, parent){

}

ProjectProcess::ResultCode VolumeDipScanProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString iname;
std::cout<<"1"<<std::endl<<std::flush;
    try{
        iname=getParam(parameters, "input");
        m_ilineDipName=getParam(parameters, "iline-dip");
        m_xlineDipName=getParam(parameters, "xline-dip");
        m_semblanceName=getParam(parameters, "semblance");
        m_windowLines=getParam(parameters, "window-lines").toInt();
        m_windowSamples=getParam(parameters, "window-samples").toInt();
        m_minimumDip=getParam(parameters, "minimum-dip").toDouble();
        m_maximumDip=getParam(parameters, "maximum-dip").toDouble();
        m_dipCount=getParam(parameters, "dip-count").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }
std::cout<<"2"<<std::endl<<std::flush;
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
std::cout<<"3"<<std::endl<<std::flush;
    m_ilineDipVolume=std::make_shared<Volume>(m_inputVolume->bounds(), m_inputVolume->domain(), VolumeType::Other, 0);
    if( !m_ilineDipVolume){
        setErrorString("Allocating iline dip volume failed!");
        return ResultCode::Error;
    }
std::cout<<"4"<<std::endl<<std::flush;
    m_xlineDipVolume=std::make_shared<Volume>(m_inputVolume->bounds(), m_inputVolume->domain(), VolumeType::Other, 0 );
    if( !m_xlineDipVolume){
        setErrorString("Allocating xline dip volume failed!");
        return ResultCode::Error;
    }
std::cout<<"5"<<std::endl<<std::flush;
    m_semblanceVolume=std::make_shared<Volume>(m_inputVolume->bounds(), m_inputVolume->domain(), VolumeType::Other, 0 );
    if( !m_semblanceVolume){
        setErrorString("Allocating semblance volume failed!");
        return ResultCode::Error;
    }
std::cout<<"6"<<std::endl<<std::flush;
    return ResultCode::Ok;
}



ProjectProcess::ResultCode VolumeDipScanProcess::run(){

    Grid3DBounds bounds=m_inputVolume->bounds();

    emit currentTask("Computing output volumes");
    emit started(bounds.ni() - 2);
    qApp->processEvents();

    // test scan only intervals
    int w_lines=m_windowLines/2;
    int w_samples=m_windowSamples/2;

    // first step scan semblance on interval
    for( int i=bounds.i1(); i<=bounds.i2(); i+=w_lines){

        for( int j=bounds.j1(); j<bounds.j2(); j+=w_lines){

            #pragma omp parallel for
            for( int k=0; k<bounds.nt(); k+=w_samples){

                scan_maximum_semblance(i,j,k);
                // testing
                //(*m_ilineDipVolume)(i,j,k)=k;
            }
        }

        emit progress( i-bounds.i1() -1 );
        qApp->processEvents();
    }

    // next step interpolate intermediate values
    interpolateIntermediate(*m_ilineDipVolume, w_lines, w_samples);
    interpolateIntermediate(*m_xlineDipVolume, w_lines, w_samples);
    interpolateIntermediate(*m_semblanceVolume, w_lines, w_samples);

    /* working old version
    // scan each and every sample
    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<bounds.j2(); j++){

            #pragma omp parallel for
            for( int k=0; k<bounds.nt(); k++){

                scan_maximum_semblance(i,j,k);

            }
        }

        emit progress( i-bounds.i1() -1 );
        qApp->processEvents();
    }
    */

    emit currentTask("Saving output volumes");
    emit started(3);
    emit progress(0);
    qApp->processEvents();

    if( !m_ilineDipName.isEmpty() && !project()->addVolume( m_ilineDipName, m_ilineDipVolume,  params() )){
        setErrorString( QString("Could not save volume \"%1\"!").arg(m_ilineDipName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    if( !m_xlineDipName.isEmpty() && !project()->addVolume( m_xlineDipName, m_xlineDipVolume,  params() )){
        setErrorString( QString("Could not save volume \"%1\"!").arg(m_xlineDipName) );
        return ResultCode::Error;
    }
    emit progress(2);
    qApp->processEvents();

    if( !m_semblanceName.isEmpty() && !project()->addVolume( m_semblanceName, m_semblanceVolume,  params() )){
        setErrorString( QString("Could not save volume \"%1\"!").arg(m_semblanceName) );
        return ResultCode::Error;
    }
    emit progress(3);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}



void VolumeDipScanProcess::scan_maximum_semblance( int i, int j, int k){

    const int W=m_windowSamples;
    std::vector<std::tuple<int, int, double>> path(m_windowLines);

    // shift per trace between -W and W

    auto t0 = m_inputVolume->bounds().sampleToTime(k);
    auto dt = m_inputVolume->dt();


     // scan inline dip
    float best_shift=0;
    auto best_semblance=std::numeric_limits<double>::lowest();
    for( int idip=0; idip<m_dipCount; idip++){

        double dip=m_minimumDip + idip*double(m_maximumDip-m_minimumDip)/(m_dipCount-1);

        for( int itr=-m_windowLines/2; itr<=m_windowLines/2; itr++ ){

            path[itr + m_windowLines/2 ]=std::make_tuple( i+itr, j, t0 + itr*dip*dt);
        }

        auto s = semblance( *m_inputVolume, path, W/2);

        if( s>best_semblance ){
            best_semblance=s;
            best_shift=dip;
        }

    }
    float ildip = best_shift;
    float ilsemb=best_semblance;

    // scan xline dip
   best_shift=0;
   best_semblance=std::numeric_limits<double>::lowest();

   for( int idip=0; idip<m_dipCount; idip++){

       double dip=m_minimumDip + idip*double(m_maximumDip-m_minimumDip)/(m_dipCount-1);

       for( int itr=-m_windowLines/2; itr<=m_windowLines/2; itr++ ){

           path[itr + m_windowLines/2 ]=std::make_tuple( i, j+itr, t0 + 0.001*itr*dip); // msec -> sec
       }

       auto s = semblance( *m_inputVolume, path, W/2);

       if( s>best_semblance ){
           best_semblance=s;
           best_shift=dip;
       }

   }
   float xldip = best_shift;
   float xlsemb=best_semblance;

   (*m_ilineDipVolume)(i,j,k)=ildip;
   (*m_xlineDipVolume)(i,j,k)=xldip;
   (*m_semblanceVolume)(i,j,k)=std::max(ilsemb,xlsemb);
}


