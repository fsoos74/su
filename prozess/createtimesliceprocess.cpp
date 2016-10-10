#include "createtimesliceprocess.h"

#include<seismicdatasetinfo.h>
#include<chrono>
#include<iostream>

#include "utilities.h"

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns" };

CreateTimesliceProcess::CreateTimesliceProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Create Timeslice"), project, parent){

}

ProjectProcess::ResultCode CreateTimesliceProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    QString dataset=parameters.value(QString("dataset"));

    if( !parameters.contains(QString("slice"))){
        setErrorString("Parameters contain no output slice!");
        return ResultCode::Error;
    }
    m_sliceName=parameters.value(QString("slice"));

    if( !parameters.contains(QString("horizon"))){

        if(!parameters.contains(QString("time"))){
            setErrorString("Parameters contain neither horizon nor time!");
            return ResultCode::Error;
        }

        m_sliceTime = parameters.value(QString("time")).toDouble();
    }
    else{
        m_horizonName=parameters.value( QString("horizon") );
        m_horizon=project()->loadGrid( GridType::Horizon, m_horizonName);
        if( !m_horizon ){
            setErrorString("Loading horizon failed!");
            return ResultCode::Error;
        }
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
    //std::cout<<"Dataset: "<<dataset.toStdString()<<std::endl;
    //std::cout<<"iline range: "<< m_reader->minInline()<<" - "<< m_reader->maxInline()<<std::endl;
    //std::cout<<"xline range: "<< m_reader->minCrossline()<<" - "<< m_reader->maxCrossline()<<std::endl;

    if( m_horizon){
         m_slice=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(m_horizon->bounds()));
    }
    else{
        Grid2DBounds bounds( m_reader->minInline(), m_reader->minCrossline(),
                           m_reader->maxInline(), m_reader->maxCrossline() );
        m_slice=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));
    }

    if( !m_slice ){
        setErrorString("Allocating slice failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CreateTimesliceProcess::run(){

    auto start = std::chrono::steady_clock::now();

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

        qreal t=0.001*m_sliceTime;  // slicetime in millisecs
        if( m_horizon){ // we are using a horizon and not constant time

            //if( !m_horizon->bounds().isInside(iline, xline)) continue; // don't need this anymore. valueAt does the job
            Grid2D<float>::value_type v=m_horizon->valueAt(iline, xline);
            if( v == m_horizon->NULL_VALUE ) continue;

            t=0.001 * v;    // horizon in millis
        }
        // interpolate between nearest samples, should add this to trace
        qreal x=(t - trace.ft() )/trace.dt();
        size_t i=std::truncf(x);
        x-=i;
        if( x>=0 || i<trace.samples().size()){
            (*m_slice)(iline, xline)=( 1.-x) * trace.samples()[i] + x*trace.samples()[i+1];
        }

        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


    std::pair<GridType, QString> sliceTypeAndName = splitFullGridName( m_sliceName );
    if( !project()->addGrid( sliceTypeAndName.first, sliceTypeAndName.second, m_slice)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_sliceName) );
        return ResultCode::Error;
    }
    emit finished();

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout<<"Process  took "<< std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

    return ResultCode::Ok;
}

