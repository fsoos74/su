#include "amplitudevolumeprocess.h"

#include "utilities.h"      // setRequiredHeaderwords

#include<seismicdatasetreader.h>
#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>
#include<trace.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "dt", "ns" };

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
    auto maxz=static_cast<int>(1000*(m_reader->info().ft()+m_reader->info().nt()*m_reader->info().dt()));   // sec -> msec
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

    m_volume=std::shared_ptr<Volume>(new Volume(m_bounds));

    return ResultCode::Ok;
}


ProjectProcess::ResultCode AmplitudeVolumeProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    auto vbounds=m_volume->bounds();

    emit started(reader->trace_count());
    reader->seek_trace(0);


    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        if( vbounds.isInside(iline, xline) ){

            for( int i=0; i<m_bounds.nt(); i++){

                double t=m_bounds.sampleToTime(i);

                // interpolate between nearest samples, should add this to trace
                qreal x=(t - trace.ft() )/trace.dt();
                size_t j=std::truncf(x);
                x-=j;
                if( x<0 ) x=0;
                if( x>=0 && j<trace.samples().size()){
                    (*m_volume)(iline, xline, i)=( 1.-x) * trace.samples()[j] + x*trace.samples()[j+1];
                }

            }

        }

        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


    if( !project()->addVolume( m_volumeName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit finished();


    return ResultCode::Ok;
}
