#include "frequencyvolumeprocess.h"

#include "utilities.h"      // setRequiredHeaderwords

#include<seismicdatasetreader.h>
#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>
#include<trace.h>
#include<fft.h>

#include<future>

#include <omp.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "dt", "ns" };

FrequencyVolumeProcess::FrequencyVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Frequency Volume"), project, parent){

}

ProjectProcess::ResultCode FrequencyVolumeProcess::init( const QMap<QString, QString>& parameters ){

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

    if( !parameters.contains(QString("minimum-frequency"))){

        setErrorString("Parameters don't contain minimum-frequency");
        return ResultCode::Error;
    }
    m_minimumFrequency=parameters.value("minimum-frequency").toDouble();

    if( !parameters.contains(QString("maximum-frequency"))){

        setErrorString("Parameters don't contain maximum-frequency");
        return ResultCode::Error;
    }
    m_maximumFrequency=parameters.value("maximum-frequency").toDouble();

    if( !parameters.contains(QString("window-samples"))){

        setErrorString("Parameters don't contain window-samples");
        return ResultCode::Error;
    }
    m_windowSamples=parameters.value("window-samples").toInt();

    if( !parameters.contains(QString("window-position"))){

        setErrorString("Parameters don't contain window-position");
        return ResultCode::Error;
    }
    m_position=toHorizonWindowPosition( parameters.value("window-position") );

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




template< typename CONST_ITERATOR >
float frequencyAmplitude( CONST_ITERATOR first, CONST_ITERATOR last, const std::vector<float>& freqs,
                          float minimumFrequency, float maximumFrequency ){
    std::vector<float> ibuf( first, last);
    // add check that ibuf.size is power of 2  and check freqs.size>=ibuf.size!!!
    auto obuf=fft(ibuf);

    float mv = 0;
    for( int i=0; i<obuf.size(); i++){

        float v = std::norm( obuf[i] );    // complex abs squared

        if( minimumFrequency<=freqs[i] && freqs[i]<=maximumFrequency) mv+=v;
    }

    return mv;
}


//void FrequencyVolumeProcess::work( )

ProjectProcess::ResultCode FrequencyVolumeProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    emit started(reader->trace_count());
    reader->seek_trace(0);
//std::cout<<"DT="<<reader->dt()<<std::endl;
    auto freqs = fft_freqs(reader->dt(), m_windowSamples);

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();
        int traceStartIndex = trace.time_to_index(m_bounds.ft());

        #pragma omp parallel for
        for( int i=0; i<m_bounds.sampleCount(); i++){  // i is output volume based

            ssize_t windowIndex=traceStartIndex+i;                                                       // trace based

            switch(m_position){
            case HorizonWindowPosition::Above: windowIndex-=m_windowSamples; break;
            case HorizonWindowPosition::Center: windowIndex-=m_windowSamples/2; break;
            case HorizonWindowPosition::Below: break;
            }

            seismic::Trace::Samples::const_iterator first=trace.samples().cbegin() + windowIndex;
            seismic::Trace::Samples::const_iterator last=trace.samples().cbegin() + windowIndex + m_windowSamples;

            if( windowIndex<0 || windowIndex+m_windowSamples>=trace.samples().size() ) continue;

            (*m_volume)(iline, xline, i)= frequencyAmplitude( first, last, freqs, m_minimumFrequency, m_maximumFrequency);
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


/* WORKING VERSION
template< typename CONST_ITERATOR >
float frequencyAmplitude( CONST_ITERATOR first, CONST_ITERATOR last, const std::vector<float>& freqs,
                          float minimumFrequency, float maximumFrequency ){
    std::vector<float> ibuf( first, last);
    // add check that ibuf.size is power of 2  and check freqs.size>=ibuf.size!!!
    auto obuf=fft(ibuf);

    float mv = 0;
    for( int i=0; i<obuf.size(); i++){

        float v = std::norm( obuf[i] );    // complex abs squared

        if( minimumFrequency<=freqs[i] && freqs[i]<=maximumFrequency) mv+=v;
    }

    return mv;
}

ProjectProcess::ResultCode FrequencyVolumeProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    emit started(reader->trace_count());
    reader->seek_trace(0);
//std::cout<<"DT="<<reader->dt()<<std::endl;
    auto freqs = fft_freqs(reader->dt(), m_windowSamples);

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        for( int i=0; i<m_bounds.sampleCount(); i++){

            ssize_t horizonIndex=i;
            ssize_t windowIndex=horizonIndex;

            switch(m_position){
            case HorizonWindowPosition::Above: windowIndex-=m_windowSamples; break;
            case HorizonWindowPosition::Center: windowIndex-=m_windowSamples/2; break;
            case HorizonWindowPosition::Below: break;
            }

            seismic::Trace::Samples::const_iterator first=trace.samples().cbegin() + windowIndex;
            seismic::Trace::Samples::const_iterator last=trace.samples().cbegin() + windowIndex + m_windowSamples;

            if( windowIndex<0 || windowIndex+m_windowSamples>=trace.samples().size() ) continue;

            (*m_volume)(iline, xline, i)= frequencyAmplitude( first, last, freqs, m_minimumFrequency, m_maximumFrequency);
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

*/


/* ORIGINAL WORKING VERSION
ProjectProcess::ResultCode FrequencyVolumeProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    emit started(reader->trace_count());
    reader->seek_trace(0);

    std::vector<float> ibuf(m_windowSamples);
    auto freqs = fft_freqs(reader->dt(), m_windowSamples);

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        for( int i=0; i<m_bounds.sampleCount(); i++){

            ssize_t horizonIndex=i;
            ssize_t windowIndex=horizonIndex;

            switch(m_position){
            case HorizonWindowPosition::Above: windowIndex-=m_windowSamples; break;
            case HorizonWindowPosition::Center: windowIndex-=m_windowSamples/2; break;
            case HorizonWindowPosition::Below: break;
            }

            if( windowIndex<0 || windowIndex+m_windowSamples>=trace.samples().size() ) continue;

            seismic::Trace::Samples::const_iterator begin=trace.samples().cbegin() + windowIndex;
            seismic::Trace::Samples::const_iterator end=trace.samples().cbegin() + windowIndex + m_windowSamples + 1;

            std::copy( begin, end, ibuf.begin() );
            auto obuf=fft(ibuf);

            float mv = 0;
            for( int i=0; i<obuf.size(); i++){

                float v = std::norm( obuf[i] );    // complex abs squared

                if( m_minimumFrequency<=freqs[i] && freqs[i]<=m_maximumFrequency) mv+=v;
            }

            (*m_volume)(iline, xline, i)=mv;

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
*/
