#include "exportvolumeprocess.h"

#include<QTransform>
#include<segyinfo.h>
#include<segywriter.h>
#include<segy_text_header.h>

#include<QtGui> // qApp->processEvents


ExportVolumeProcess::ExportVolumeProcess(AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Export Volume"), project, parent){

}

ProjectProcess::ResultCode ExportVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        m_volumeName=getParam(parameters,"volume");
        m_outputFilename=getParam(parameters, "output-file");
        m_nullValue=getParam(parameters, "null-value").toFloat();
        auto desc=getParam(parameters, "description");
        m_description=desc.split("\n", QString::SkipEmptyParts);
        m_volume=project()->loadVolume(m_volumeName);
        if( !m_volume ){
            setErrorString("Loading volume failed!");
            return ResultCode::Error;
        }

        Grid3DBounds bounds=m_volume->bounds();

        m_minInline = getParam(parameters, "min-inline", false, QString::number(bounds.i1())).toInt();
        m_maxInline = getParam(parameters, "max-inline", false, QString::number(bounds.i2())).toInt();
        m_minCrossline = getParam(parameters, "min-crossline", false, QString::number(bounds.j1())).toInt();
        m_maxCrossline = getParam(parameters, "max-crossline", false, QString::number(bounds.j2())).toInt();
        m_minTime = getParam(parameters, "min-time", false, QString::number(bounds.ft())).toFloat();
        m_maxTime = getParam(parameters, "max-time", false, QString::number(bounds.lt())).toFloat();
   }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

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

            int cdp=1 + (iline-bounds.i1())*bounds.nj() + xline - bounds.j1();

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

                if( s==m_volume->NULL_VALUE ){
                    s=m_nullValue;
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
    for(auto str : m_description){
        textHeaderStr.push_back(str.toStdString());
    }
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
