#include "replacebadtracesprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

ReplaceBadTracesProcess::ReplaceBadTracesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Replace Bad Traces"), project, parent){

}

ProjectProcess::ResultCode ReplaceBadTracesProcess::init( const QMap<QString, QString>& parameters ){

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

    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


std::shared_ptr<seismic::Trace> ReplaceBadTracesProcess::getTrace( int il, int xl){

    return m_reader->readFirstTrace( QString("iline"), QString::number(il), QString("xline"), QString::number(xl) );
}

bool ReplaceBadTracesProcess::isBad(const seismic::Trace& trc){

    for( auto s : trc.samples() ){
        if( s==-10000000) return true;
    }

    return false;
}

ProjectProcess::ResultCode ReplaceBadTracesProcess::run(){


    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               m_reader->info().dimensions(), m_reader->info().domain(), m_reader->info().mode(),
                                                               m_reader->info().ft(), m_reader->info().dt(), m_reader->info().nt() );

    m_writer = std::make_shared<SeismicDatasetWriter>( dsinfo );


    auto segy_reader=m_reader->segyReader();
    emit started(segy_reader->trace_count());

    int n_bad=0;

    while( segy_reader->current_trace()<segy_reader->trace_count()){

        seismic::Trace trace=segy_reader->read_trace();
        if( !isBad( trace)){
            m_writer->writeTrace(trace);   // copy to output
            //std::cout<<segy_reader->current_trace()<<" of "<<segy_reader->trace_count()<<std::endl;
        }
        else{

            auto oldpos=segy_reader->current_trace();
            const seismic::Header& header=trace.header();
            int il=header.at("iline").intValue();
            int xl=header.at("xline").intValue();
            n_bad++;
            std::cout<<"bad trace #"<<n_bad<<" at il="<<il<<" xl="<<xl<<std::endl;

            // found a bad trace, now replace it with mean of adjacent traces
            seismic::Trace::Samples sam(trace.samples().size(), 0);
            int n=0;
            for( auto ii=il-1; ii<=il+1; ii++){

                for( auto xx=xl-1; xx<=xl+1; xx++){

                    auto trc=getTrace(ii,xx);
                    if( !trc ) continue;

                    //don't use other bad traces!!! this also excludes the current trace
                    if( isBad(*trc)) continue;

                    for( auto k=0; k<sam.size(); k++ ){
                        sam[k]+=trc->samples()[k];
                    }
                    n++;
                }
            }

            for( auto k=0; k<sam.size(); k++){
                sam[k]/=n;
            }

            m_writer->writeTrace( seismic::Trace( trace.ft(), trace.dt(), sam, trace.header()) );

            segy_reader->seek_trace(oldpos+1);

        }

        emit progress( segy_reader->current_trace() );
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
