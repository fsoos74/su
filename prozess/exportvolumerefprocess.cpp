#include "exportvolumerefprocess.h"

#include<segyinfo.h>
#include<segywriter.h>
#include<segy_text_header.h>
#include<seismicdatasetreader.h>
#include<QtGui> // qApp->processEvents


ExportVolumeRefProcess::ExportVolumeRefProcess(AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Export Volume"), project, parent){

}

ProjectProcess::ResultCode ExportVolumeRefProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_volumeName=getParam(parameters,"volume");
        m_outputFilename=getParam(parameters, "output-file");
        m_nullValue=getParam(parameters, "null-value").toFloat();
        auto ebcdic=getParam(parameters, "ebcdic");
        m_ebcdic=ebcdic.split("\n", QString::SkipEmptyParts);
        m_refStackName=getParam(parameters, "ref-stack");


   }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode ExportVolumeRefProcess::run(){


    auto volume=project()->loadVolume(m_volumeName);
    if( !volume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    auto reader=project()->openSeismicDataset(m_refStackName);
    if(!reader){
        setErrorString("Open reference stack failed!");
        return ResultCode::Error;
    }

    //only IEEE implemented
    seismic::SEGYInfo info=reader->segyReader()->info();
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //info.setScalco(-1./SCALCO);
    //info.setSwap(true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    //info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );

    auto bh= reader->segyReader()->binaryHeader();
    bh["format"]=seismic::HeaderValue::makeIntValue(static_cast<int>(seismic::SEGYSampleFormat::IEEE));

    seismic::SEGYTextHeaderStr ehdr;
    for(auto str : m_ebcdic){
        ehdr.push_back(str.toStdString());
    }
    auto ebcdic=seismic::convertToRaw(ehdr);

    std::unique_ptr<seismic::SEGYWriter> writer( new seismic::SEGYWriter(
                m_outputFilename.toStdString(), info, ebcdic , bh) );
    writer->write_leading_headers();


    while(reader->good()){
        auto trace=reader->readTrace();
        auto header=trace.header();
        //std::cout<<"tracf="<<header["tracf"].intValue()<<" il="<<header["iline"].intValue()<<" xl="<<header["xline"].intValue()
        //        <<" cdp="<<header["cdp"].intValue()<<" x="<<header["cdpx"].floatValue()<<" y="<<header["cdpy"].floatValue()<<std::endl<<std::flush;
        auto il=static_cast<size_t>(header["iline"].intValue());
        auto xl=static_cast<size_t>(header["xline"].intValue());
        for( int i=0; i<trace.size(); i++){
            auto t=trace.ft()+i*trace.dt();
            auto v=volume->value(il,xl,t);
            if(v==volume->NULL_VALUE) v=m_nullValue;
            trace[i]=v;
        }
        writer->write_trace(trace);
    }
    return ResultCode::Ok;
}


