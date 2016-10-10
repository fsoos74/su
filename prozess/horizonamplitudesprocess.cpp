#include "horizonamplitudesprocess.h"

#include <seismicdatasetreader.h>

#include "utilities.h"

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns" };

HorizonAmplitudesProcess::HorizonAmplitudesProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Horizon Amplitudes"), project, parent){

}

ProjectProcess::ResultCode HorizonAmplitudesProcess::init( const QMap<QString, QString>& parameters ){

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

    if( !parameters.contains(QString("method"))){

        setErrorString("Parameters contain no summation method!");
        return ResultCode::Error;
    }
    m_method=string2ReductionMethod( parameters.value("method"));

    if( !parameters.contains(QString("half-samples"))){

        setErrorString("Parameters contain no operator length!");
        return ResultCode::Error;
    }
    m_halfSamples=parameters.value("half-samples").toUInt();

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

    m_reductionFunction=reductionFunctionFactory(m_method);


    return ResultCode::Ok;
}

ProjectProcess::ResultCode HorizonAmplitudesProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    emit started(reader->trace_count());
    reader->seek_trace(0);

    int n_null=0;
    int n_out=0;

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        //if( !m_horizon->bounds().isInside(iline, xline)) continue;  //valueAt does the job
        Grid2D<float>::value_type v=m_horizon->valueAt(iline, xline);
        if( v != m_horizon->NULL_VALUE ){

            qreal t=0.001 * v;      // horizon is in millis

            size_t horizonIndex=trace.time_to_index(t); // returns trace.samples.size() if time is out of trace bounds

            if( ( horizonIndex >= m_halfSamples ) && ( horizonIndex + m_halfSamples <trace.samples().size() ) ){
                seismic::Trace::Samples::const_iterator begin=trace.samples().cbegin() + horizonIndex - m_halfSamples;
                seismic::Trace::Samples::const_iterator end=trace.samples().cbegin() + horizonIndex + m_halfSamples + 1;
                (*m_grid)(iline, xline)= (*m_reductionFunction)(begin, end);
            }else{
                n_out++;
                //std::cout<<"!!! "<<horizonIndex<<" out of "<<m_halfSamples<<" - "<<trace.samples().size()<<std::endl;
            }
       }else{
            n_null++;
        }
        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    std::cout<<"Number of null horizon values: "<<n_null<<std::endl;
    std::cout<<"Number of out of trace windows: "<<n_out<<std::endl;
    std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_gridName );
    if( !project()->addGrid( gridTypeAndName.first, gridTypeAndName.second, m_grid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_gridName) );
        return ResultCode::Error;
    }
    emit finished();

    return ResultCode::Ok;
}


