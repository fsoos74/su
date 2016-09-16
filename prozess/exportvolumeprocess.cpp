#include "exportvolumeprocess.h"

#include<QTransform>
#include<segyinfo.h>
#include<segywriter.h>
#include<segy_text_header.h>

#include<QtGui> // qApp->processEvents


ExportVolumeProcess::ExportVolumeProcess(std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Export Seismic Dataset"), project, parent){

}

ProjectProcess::ResultCode ExportVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("volume"))){
        setErrorString("Parameters contain no volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("volume"));

    m_volume=project()->loadVolume(m_volumeName);
    if( !m_volume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    Grid3DBounds bounds=m_volume->bounds();

    m_minInline=bounds.inline1();
    if( parameters.contains(QString("min-inline"))){
        m_minInline=std::max(m_minInline, parameters.value(QString("min-inline")).toInt() );
    }

    m_maxInline=bounds.inline2();
    if( parameters.contains(QString("max-inline"))){
        m_maxInline=std::min(m_maxInline, parameters.value(QString("max-inline")).toInt() );
    }

    m_minCrossline=bounds.crossline1();
    if( parameters.contains(QString("min-crossline"))){
        m_minCrossline=std::max(m_minCrossline, parameters.value(QString("min-crossline")).toInt() );
    }

    m_maxCrossline=bounds.crossline2();
    if( parameters.contains(QString("max-crossline"))){
        m_maxCrossline=std::min(m_maxCrossline, parameters.value(QString("max-crossline")).toInt() );
    }

    m_minTime=bounds.ft();
    if( parameters.contains(QString("min-time"))){
        m_minTime=std::max(m_minTime, parameters.value(QString("min-time")).toFloat() );
    }

    m_maxTime=bounds.lt();
    if( parameters.contains(QString("max-time"))){
        m_maxTime=std::min(m_maxTime, parameters.value(QString("max-time")).toFloat() );
    }


    if( !parameters.contains(QString("output-file"))){
        setErrorString("Parameters contain no output file!");
        return ResultCode::Error;
    }
    m_outputFilename=parameters.value(QString("output-file"));



    return ResultCode::Ok;
}


ProjectProcess::ResultCode ExportVolumeProcess::run(){

    const int SCALCO=-10;

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !project()->geometry().computeTransforms( XYToIlXl, IlXlToXY)){

        setErrorString("Invalid geometry, failed to compute transformations!");
        return ResultCode::Error;
    }


    Grid3DBounds bounds=m_volume->bounds();

    int i1=bounds.timeToSample(m_minTime);
    int i2=bounds.timeToSample(m_maxTime);
    Q_ASSERT(i2>=i1);
    size_t ns=static_cast<size_t>(i2-i1+1);
    size_t dt=static_cast<size_t>(1000000*bounds.dt());   // make microseconds

    seismic::SEGYTextHeader textHeader=buildTextHeader();
    seismic::Header bhdr=buildBinaryHeader(dt, ns);


    seismic::SEGYInfo info;
    info.setScalco(-1./SCALCO);
    info.setSwap(true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );
    std::unique_ptr<seismic::SEGYWriter> writer( new seismic::SEGYWriter(
                m_outputFilename.toStdString(), info, textHeader, bhdr ) );
    writer->write_leading_headers();


    seismic::Trace trace(m_minTime, bounds.dt(), ns );
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();

    thdr["delrt"]=seismic::HeaderValue::makeIntValue(1000*m_minTime);
    thdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    thdr["dt"]=seismic::HeaderValue::makeUIntValue(dt);
    thdr["scalco"]=seismic::HeaderValue::makeIntValue(SCALCO);

    emit started( (1 + m_maxInline - m_minInline ) * ( 1 + m_maxCrossline - m_minCrossline) );

    int n=0;
    for( int iline=m_minInline; iline<=m_maxInline; iline++){

        for( int xline=m_minCrossline; xline<=m_maxCrossline; xline++){

            n++;

            QPointF p=IlXlToXY.map( QPoint(iline, xline));
            qreal x=p.x();
            qreal y=p.y();

            int cdp=1 + (iline-bounds.inline1())*bounds.crosslineCount() + xline - bounds.crossline1();

            thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);                                        // XXX, maybe read from db once
            thdr["tracr"]=seismic::HeaderValue::makeIntValue(n);
            thdr["tracl"]=seismic::HeaderValue::makeIntValue(n);
            thdr["iline"]=seismic::HeaderValue::makeIntValue(iline);
            thdr["xline"]=seismic::HeaderValue::makeIntValue(xline);
            thdr["sx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["sy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["gx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["gy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["cdpx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["cdpy"]=seismic::HeaderValue::makeFloatValue(y);

            for( int i=i1; i<=i2; i++){

                float s=(*m_volume)(iline,xline,i);

                // write zero for NULL values, maybe make this selectable?
                if( s==m_volume->NULL_VALUE ){
                    s=0;
                }

                samples[i-i1]=s;
            }

            writer->write_trace(trace);
        }

        emit progress( n );

        //qApp->processEvents();

        if( isCanceled() ) return ResultCode::Canceled;
    }


    emit finished();

    return ResultCode::Ok;
}

seismic::SEGYTextHeader ExportVolumeProcess::buildTextHeader()const{

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("SEGY created with AVO-Detect");
    textHeaderStr.push_back(QString("Content:     Volume %1").arg(m_volumeName).toStdString());
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(m_minInline).arg(m_maxInline).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(m_minCrossline).arg(m_maxCrossline).toStdString());
    textHeaderStr.push_back(QString("Time [ms]:   %1 - %2").arg(1000*m_minTime).arg(1000*m_maxTime).toStdString());
    textHeaderStr.push_back("");
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Time of first sample [ms]  bytes 109-110");
    textHeaderStr.push_back("Inline                     bytes 189-192");
    textHeaderStr.push_back("Crossline                  bytes 193-196");
    textHeaderStr.push_back("X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("Y-Coordinate               bytes 185-188");

    return seismic::convertToRaw(textHeaderStr);
}

seismic::Header ExportVolumeProcess::buildBinaryHeader( size_t dt_us, size_t ns )const{

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(dt_us);

    return bhdr;
}
