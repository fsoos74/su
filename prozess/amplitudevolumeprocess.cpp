#include "amplitudevolumeprocess.h"

#include "utilities.h"      // setRequiredHeaderwords

#include<seismicdatasetreader.h>
#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>
#include<trace.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "dt", "ns" };

AmplitudeVolumeProcess::AmplitudeVolumeProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Amplitude Volume"), project, parent){

}

ProjectProcess::ResultCode AmplitudeVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    QString dataset=parameters.value(QString("dataset"));

    if( !parameters.contains(QString("volume"))){
        setErrorString("Parameters contain no output volume name!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("volume"));

    m_reader= project()->openSeismicDataset( dataset);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(dataset) );
        return ResultCode::Error;
    }
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }

    double ft=0;    // need to get this from dataset
    double dt=m_reader->segyReader()->dt();
    int nt=m_reader->segyReader()->nt();        // from binary header, need to call this after open
    int startMillis=static_cast<int>(1000*ft);
    int endMillis=static_cast<int>(1000*(ft+nt*dt) );

    if( parameters.contains("window-mode")){

        startMillis=parameters.value("window-start").toInt();
        endMillis=parameters.value("window-end").toInt();

        if( startMillis>endMillis) std::swap(startMillis, endMillis);

        if( startMillis<0 ) startMillis=0;

        nt=static_cast<int>(0.001*(endMillis-startMillis)/dt)+1;
    }


    m_bounds=Grid3DBounds( m_reader->minInline(), m_reader->maxInline(),
                           m_reader->minCrossline(), m_reader->maxCrossline(),
                           nt, 0.001*startMillis, dt);

    m_volume=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));

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

    emit started(reader->trace_count());
    reader->seek_trace(0);

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        for( int i=0; i<m_bounds.sampleCount(); i++){

            double t=m_bounds.sampleToTime(i);

            // interpolate between nearest samples, should add this to trace
            qreal x=(t - trace.ft() )/trace.dt();
            size_t j=std::truncf(x);
            x-=j;
            if( x<0 ) x=0;
            if( x>=0 && j<trace.samples().size()){
                (*m_volume)(iline, xline, i)=( 1.-x) * trace.samples()[j] + x*trace.samples()[j+1];
            }
            else{
                std::cout<<"t="<<t<<"x="<<x<<" j="<<j<<std::endl;
            }
            // no else required because volume is filled with NULL

        }

        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


    if( !project()->addVolume( m_volumeName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit finished();


    return ResultCode::Ok;
}
