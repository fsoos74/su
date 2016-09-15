#include "exportseismicprocess.h"

#include<climits>
#include<segyinfo.h>
#include<segyreader.h>
#include<segywriter.h>
#include<segy_text_header.h>
#include<segy_header_def.h>

ExportSeismicProcess::ExportSeismicProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Export Seismic Dataset"), project, parent){

}


ProjectProcess::ResultCode ExportSeismicProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    m_datasetName=parameters.value(QString("dataset"));

    m_reader= project()->openSeismicDataset( m_datasetName);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(m_datasetName) );
        return ResultCode::Error;
    }

    if( parameters.contains(QString("min-inline"))){
        m_minInline=parameters.value(QString("min-inline")).toInt();
    }
    else{
        m_minInline=m_reader->minInline();
    }

    if( parameters.contains(QString("max-inline"))){
        m_maxInline=parameters.value(QString("max-inline")).toInt();
    }
    else{
        m_maxInline=m_reader->maxInline();
    }

    if( parameters.contains(QString("min-crossline"))){
        m_minCrossline=parameters.value(QString("min-crossline")).toInt();
    }
    else{
        m_minCrossline=m_reader->minCrossline();
    }

    if( parameters.contains(QString("max-Crossline"))){
        m_maxCrossline=parameters.value(QString("max-crossline")).toInt();
    }
    else{
        m_maxCrossline=m_reader->maxCrossline();
    }


    if( !parameters.contains(QString("output-file"))){
        setErrorString("Parameters contain no output file!");
        return ResultCode::Error;
    }
    m_outputFilename=parameters.value(QString("output-file"));



    return ResultCode::Ok;
}

ProjectProcess::ResultCode ExportSeismicProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }


    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("SEGY created with AVO-Detect");
    textHeaderStr.push_back(QString("Content:     Dataset %1").arg(m_datasetName).toStdString());
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(m_minInline).arg(m_maxInline).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(m_minCrossline).arg(m_maxCrossline).toStdString());
    textHeaderStr.push_back("");
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Inline                     bytes 189-192");
    textHeaderStr.push_back("Crossline                  bytes 193-196");
    seismic::SEGYTextHeader textHeader=seismic::convertToRaw(textHeaderStr);

    seismic::Header bhdr=reader->binaryHeader();
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );

    const int SCALCO=1;   // need to get this from input ds
    seismic::SEGYInfo info;
    info.setScalco(SCALCO);
    info.setSwap(true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );
    std::unique_ptr<seismic::SEGYWriter> writer( new seismic::SEGYWriter(
                m_outputFilename.toStdString(), info, textHeader, bhdr ) );
    writer->write_leading_headers();

    emit started(reader->trace_count());
    reader->seek_trace(0);

    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        // skip traces outside area
        if( iline<m_minInline || iline > m_maxInline ||
                xline<m_minCrossline || xline>m_maxCrossline) continue;


        writer->write_trace( trace );  // throws FormatError on failure


        emit progress( reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }

    emit finished();

    return ResultCode::Ok;
}
