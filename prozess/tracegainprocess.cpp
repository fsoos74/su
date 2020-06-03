#include "tracegainprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

TraceGainProcess::TraceGainProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Offset Stack"), project, parent){

}

ProjectProcess::ResultCode TraceGainProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("output-dataset") );
       m_inputName=getParam( parameters, QString("input-dataset") );
       m_factor=getParam( parameters, QString("factor") ).toFloat();
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

    return ResultCode::Ok;
}


ProjectProcess::ResultCode TraceGainProcess::run(){

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               m_reader->info().dimensions(),
                                                               m_reader->info().domain(),
                                                               m_reader->info().mode(),
                                                               m_reader->info().ft(),
                                                               m_reader->info().dt(),
                                                               m_reader->info().nt());

    m_writer = std::make_shared<SeismicDatasetWriter>( dsinfo );

    emit started(m_reader->sizeTraces());
    m_reader->seekTrace(0);

    while( m_reader->good()){

        seismic::Trace trace=m_reader->readTrace();
        for( auto& sample : trace.samples()){
            if(std::isfinite(sample))
                sample=m_factor*sample;
        }
        m_writer->writeTrace(trace);

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
