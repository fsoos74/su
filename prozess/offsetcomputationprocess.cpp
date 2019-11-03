#include "offsetcomputationprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

OffsetComputationProcess::OffsetComputationProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Offset Stack"), project, parent){

}

ProjectProcess::ResultCode OffsetComputationProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("output-dataset") );
       m_inputName=getParam( parameters, QString("input-dataset") );
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


ProjectProcess::ResultCode OffsetComputationProcess::run(){

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               m_reader->info().dimensions(), m_reader->info().domain(), SeismicDatasetInfo::Mode::Prestack,
                                                               m_reader->info().ft(), m_reader->info().dt(), m_reader->info().nt());

    m_writer = std::make_shared<SeismicDatasetWriter>( dsinfo );

    emit started(m_reader->sizeTraces());
    m_reader->seekTrace(0);

    while( m_reader->good()){

        seismic::Trace trace=m_reader->readTrace();
        const seismic::Header& header=trace.header();
        auto sx=header.at("sx").floatValue();
        auto sy=header.at("sy").floatValue();
        auto gx=header.at("gx").floatValue();
        auto gy=header.at("gy").floatValue();
        auto offset=std::sqrt(std::pow(sx-gx,2)+std::pow(sy-gy,2));
        auto new_header=header;
        new_header["offset"]=seismic::HeaderValue::makeFloatValue(offset);
        seismic::Trace new_trace(trace.ft(), trace.dt(), trace.samples(), new_header );
        m_writer->writeTrace(new_trace);

        emit progress( m_reader->tellTrace());
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
