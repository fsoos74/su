#include "stacktogatherprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

StackToGatherProcess::StackToGatherProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Stack to Gather"), project, parent){

}

ProjectProcess::ResultCode StackToGatherProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("gather") );
       m_inputNames=unpackParamList( getParam( parameters, QString("stacks") ) );
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }
/*
    m_reader= project()->openSeismicDataset( m_inputName );
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(m_inputName) );
        return ResultCode::Error;
    }

    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Prestack ){
        setErrorString("This process was designed for unstacked data!");
        return ResultCode::Error;
    }
*/
    return ResultCode::Ok;
}


ProjectProcess::ResultCode StackToGatherProcess::run(){

    std::vector<std::shared_ptr<SeismicDatasetReader>> stack_readers;
    for( auto name : m_inputNames){
        auto reader = project()->openSeismicDataset(name);
        if( !reader){
            setErrorString(tr("Open dataset \"%1\" failed!").arg(name));
            return ResultCode::Error;
        }
        if(stack_readers.size()>1){ // check if stack matches first stack
            if( reader->info().dimensions() != stack_readers.front()->info().dimensions()
                    || reader->info().domain() != stack_readers.front()->info().domain()
                    || reader->info().ft() != stack_readers.front()->info().ft()
                    || reader->info().dt() != stack_readers.front()->info().dt()
                    || reader->info().nt() != stack_readers.front()->info().nt()

                    ){
                setErrorString(tr("stack \"%\" does not match first stack!").arg(name));
                return ResultCode::Error;
            }
        }
        stack_readers.push_back(reader);
    }

    if( stack_readers.size()<1){
        setErrorString("No input stacks!");
        return ResultCode::Error;
    }

    auto rinfo=stack_readers.front()->info();
    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               rinfo.dimensions(), rinfo.domain(), SeismicDatasetInfo::Mode::Prestack,
                                                               rinfo.ft(), rinfo.dt(), rinfo.nt());
    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );
    if( !writer){
        setErrorString("Open dataset writer failed!");
        return ResultCode::Error;
    }

    auto reader0=stack_readers.front();
    emit started(reader0->sizeTraces());
    reader0->seekTrace(0);
    while ( reader0->good()) {

        auto trace0=reader0->readTrace();
        const seismic::Header& header0=trace0.header();
        auto cdp=header0.at("cdp").intValue();

        writer->writeTrace(trace0);

        for(int i=1; i<stack_readers.size(); i++){
            auto reader1=stack_readers[i];
            while(reader1->good()){
                auto trace1 = reader1->readTrace();
                const seismic::Header& header1=trace1.header();
                auto cdp1=header1.at("cdp").intValue();
                if( cdp1<cdp ) continue;  // search for cdp, data is assumed sorted so it has to be after this trace
                if( cdp1>cdp ){           // we are past the cdp, put trace back for later
                    reader1->seekTrace(reader1->tellTrace()-1);
                    break;
                }
                writer->writeTrace(trace1);
            }
        }

        emit progress( reader0->tellTrace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    writer->close();

    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    emit finished();

    return ResultCode::Ok;
}
