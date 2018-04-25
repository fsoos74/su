#include "offsetstackprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

OffsetStackProcess::OffsetStackProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Offset Stack"), project, parent){

}

ProjectProcess::ResultCode OffsetStackProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("output-dataset") );
       m_inputName=getParam( parameters, QString("input-dataset") );
       m_minOffset=getParam( parameters, QString("min-offset") ).toFloat();
       m_maxOffset=getParam( parameters, QString("max-offset") ).toFloat();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_reader= project()->openSeismicDataset( m_inputName );
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(m_inputName) );
        return ResultCode::Error;
    }

    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Prestack ){
        setErrorString("This process was designed for unstacked data!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode OffsetStackProcess::run(){


    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               m_reader->info().dimensions(), m_reader->info().domain(), SeismicDatasetInfo::Mode::Poststack,
                                                               m_reader->info().ft(), m_reader->info().dt(), m_reader->info().nt());

    m_writer = std::make_shared<SeismicDatasetWriter>( dsinfo );

    emit started(reader->trace_count());
    reader->seek_trace(0);

    int stack_cdp=-1;
    int stack_fold=0;
    seismic::Trace::time_type   stack_ft;
    seismic::Trace::time_type   stack_dt;
    seismic::Trace::Samples     stack_samples;
    seismic::Header             stack_header;

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int cdp=header.at("cdp").intValue();
        float offset=header.at("offset").floatValue();

        if( cdp!=stack_cdp ){

            if( stack_fold ){

                // divide by fold and store fold i header nhs
                for( auto it=stack_samples.begin(); it!=stack_samples.end(); ++it){
                    *it/=stack_fold;
                }
                stack_header["nhs"]=seismic::HeaderValue::makeIntValue(stack_fold);
                stack_header["offset"]=seismic::HeaderValue::makeFloatValue(0);
                seismic::Trace stack_trace( stack_ft, stack_dt, stack_samples, stack_header );
                m_writer->writeTrace(stack_trace);
            }

            stack_cdp=cdp;
            stack_fold=0;
            stack_header=trace.header();
            stack_samples = seismic::Trace::Samples( trace.samples().size(), 0);
            stack_ft = trace.ft();
            stack_dt = trace.dt();
        }

        if( offset<m_minOffset || offset>m_maxOffset) continue;

        for( auto i=0; i<stack_samples.size(); i++){
            stack_samples[i]+=trace.samples()[i];
        }

        stack_fold++;

        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    m_writer->close();

    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    emit finished();

    return ResultCode::Ok;
}
