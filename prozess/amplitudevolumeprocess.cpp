#include "amplitudevolumeprocess.h"

#include "utilities.h"      // setRequiredHeaderwords

#include<seismicdatasetreader.h>
#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>
#include<trace.h>


AmplitudeVolumeProcess::AmplitudeVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Amplitude Volume"), project, parent){

}

ProjectProcess::ResultCode AmplitudeVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString dataset;
    bool crop;
    int cminil;
    int cmaxil;
    int cminxl;
    int cmaxxl;
    int cminz;
    int cmaxz;

    try{
        dataset=getParam(parameters, "dataset");
        m_volumeName=getParam(parameters, "volume");
        crop=static_cast<bool>(getParam(parameters, "crop").toInt());
        cminil=getParam(parameters, "min-iline").toInt();
        cmaxil=getParam(parameters, "max-iline").toInt();
        cminxl=getParam(parameters, "min-xline").toInt();
        cmaxxl=getParam(parameters, "max-xline").toInt();
        cminz=getParam(parameters, "min-z").toInt();
        cmaxz=getParam(parameters, "max-z").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_reader= project()->openSeismicDataset( dataset);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(dataset) );
        return ResultCode::Error;
    }
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }

    auto dz=1000*m_reader->info().dt();          // sec -> msec
    auto minz=static_cast<int>( 1000*m_reader->info().ft()); // sec -> msec
    auto maxz=static_cast<int>(1000*(m_reader->info().ft()+(m_reader->info().nt()-1)*m_reader->info().dt()));   // sec -> msec
    auto minil=m_reader->minInline();
    auto maxil=m_reader->maxInline();
    auto minxl=m_reader->minCrossline();
    auto maxxl=m_reader->maxCrossline();

    if( crop ){

        minil=std::max(minil, cminil);
        maxil=std::min(maxil, cmaxil);
        minxl=std::max(minxl, cminxl);
        maxxl=std::min(maxxl, cmaxxl);
        minz=std::max(minz, cminz);
        maxz=std::min(maxz, cmaxz);
    }

    auto nz = static_cast<int>(std::round(double(maxz-minz)/dz+1));

    m_bounds=Grid3DBounds( minil, maxil, minxl, maxxl, nz, 0.001*minz, 0.001*dz );  // msec -> sec

#ifdef IO_VOLUMES
    m_volumeWriter=project()->openVolumeWriter(m_volumeName, m_bounds, Domain::Other, VolumeType::Other );
    if( !m_volumeWriter){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }
    m_volumeWriter->setParent(this);
#else
    m_volume=std::make_shared<Volume>(m_bounds);
    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }
#endif

    return ResultCode::Ok;
}


ProjectProcess::ResultCode AmplitudeVolumeProcess::run(){

    emit started(m_reader->sizeTraces());
    m_reader->seekTrace(0);

    Grid3DBounds vbounds;
    std::shared_ptr<Volume> volume;

#ifdef IO_VOLUMES
    vbounds=Grid3DBounds(0,0, m_bounds.j1(),m_bounds.j2(), m_bounds.nt(),m_bounds.ft(),m_bounds.dt());
    volume.reset();
#else
    vbounds=m_volume->bounds();
    volume=m_volume;
#endif

    while( m_reader->good() ){

        seismic::Trace trace=m_reader->readTrace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

#ifdef IO_VOLUMES
        if( iline>vbounds.i2()){ // finished chunk
            if(volume){  // save old
                if( !m_volumeWriter->write(*volume)){
                    setErrorString(QString("Writing chunk failed: %1").arg(m_volumeWriter->lastError()));
                    return ResultCode::Error;
                }
            }

            int il1=iline;
            int il2=std::min(il1+CHUNK_SIZE, m_bounds.i2());
            vbounds=Grid3DBounds(il1,il2, m_bounds.j1(),m_bounds.j2(), m_bounds.nt(),m_bounds.ft(),m_bounds.dt());
            volume=std::make_shared<Volume>(vbounds);
            if( !volume){
                setErrorString("Allocating subvolume failed!");
                return ResultCode::Error;
            }
        }
#endif

        if( vbounds.isInside(iline, xline) ){

            for( int i=0; i<m_bounds.nt(); i++){

                double t=m_bounds.sampleToTime(i);

                // interpolate between nearest samples, should add this to trace
                qreal x=(t - trace.ft() )/trace.dt();
                size_t j=std::truncf(x);
                x-=j;
                if( x<0 ) x=0;
                if( x>=0 && j<trace.samples().size()){
                    (*volume)(iline, xline, i)=( 1.-x) * trace.samples()[j] + x*trace.samples()[j+1];
                }

            }

        }

        emit progress( m_reader->tellTrace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


#ifdef IO_VOLUMES
    if(volume){  // save last chunk if necessary
        if( !m_volumeWriter->write(*volume)){
            setErrorString("Writing chunk failed!");
            return ResultCode::Error;
        }
    }
    m_volumeWriter->flush();
    m_volumeWriter->close();
    if( !project()->addVolume( m_volumeName, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
#else
    if( !project()->addVolume( m_volumeName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
#endif

    emit finished();


    return ResultCode::Ok;
}
