#include "horizonfrequenciesprocess.h"

#include <seismicdatasetreader.h>

#include "utilities.h"

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns" };

HorizonFrequenciesProcess::HorizonFrequenciesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Horizon Frequencies"), project, parent){

}

ProjectProcess::ResultCode HorizonFrequenciesProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    QString dataset=parameters.value(QString("dataset"));

    if( !parameters.contains(QString("grid"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_gridName=parameters.value(QString("grid"));

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

    if( !parameters.contains(QString("horizon"))){

        setErrorString("Parameters contain no horizon!");
        return ResultCode::Error;
    }

    m_horizonName=parameters.value( QString("horizon") );
    m_horizon=project()->loadGrid( GridType::Horizon, m_horizonName);
    if( !m_horizon ){
        setErrorString("Loading horizon failed!");
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


    Grid2DBounds bounds( m_reader->minInline(), m_reader->minCrossline(),
                       m_reader->maxInline(), m_reader->maxCrossline() );
    m_grid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));


    if( !m_grid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    //m_reductionFunction=reductionFunctionFactory(m_method);


    return ResultCode::Ok;
}

ProjectProcess::ResultCode HorizonFrequenciesProcess::run(){
/*
    std::cout<<"window-position="<<toQString(m_position).toStdString()<<std::endl;
    std::cout<<"window samples="<<m_windowSamples<<std::endl;
    std::cout<<"minimum frequency="<<m_minimumFrequency<<std::endl;
    std::cout<<"maximum frequency="<<m_maximumFrequency<<std::endl;
*/
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

        // put this here to allow using continue later on
        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;


        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        //if( !m_horizon->bounds().isInside(iline, xline)) continue;  //valueAt does the job
        Grid2D<float>::value_type v=m_horizon->valueAt(iline, xline);
        if( v == m_horizon->NULL_VALUE ) continue;  // can do this because progress is updated before
        qreal t=0.001 * v;      // horizon is in millis

        std::vector<float> ibuf(m_windowSamples);
        auto freqs = fft_freqs(trace.dt(), m_windowSamples);
        ssize_t horizonIndex=trace.time_to_index(t); // returns trace.samples.size() if time is out of trace bounds
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

        (*m_grid)(iline, xline)= mv;

    }

    std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_gridName );
    if( !project()->addGrid( gridTypeAndName.first, gridTypeAndName.second, m_grid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_gridName) );
        return ResultCode::Error;
    }
    emit finished();

    return ResultCode::Ok;
}


