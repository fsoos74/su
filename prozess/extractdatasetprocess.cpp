#include "extractdatasetprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include <volumereader2.h>
#include<iostream>

ExtractDatasetProcess::ExtractDatasetProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Extract Dataset"), project, parent){

}

ProjectProcess::ResultCode ExtractDatasetProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("output-dataset") );
       m_inputName=getParam( parameters, QString("input-volume") );
       m_il1=getParam( parameters, QString("min-iline") ).toInt();
       m_il2=getParam( parameters, QString("max-iline") ).toInt();
       m_xl1=getParam( parameters, QString("min-xline") ).toInt();
       m_xl2=getParam( parameters, QString("max-xline") ).toInt();
       m_msec1=getParam( parameters, QString("min-msec") ).toInt();
       m_msec2=getParam( parameters, QString("max-msec") ).toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }
/*
    m_inputVolume=project()->loadVolume( m_inputName);
    if( !m_inputVolume ){
        setErrorString("Loading input volume failed!");
        return ResultCode::Error;
    }
*/
    return ResultCode::Ok;
}


#include<iostream>

ProjectProcess::ResultCode ExtractDatasetProcess::run(){

    const int SCALCO=-10;

    auto reader = project()->openVolumeReader(m_inputName);
    if( !reader){
        setErrorString("Open volume reader failed!");
        return ResultCode::Error;
    }

    Grid3DBounds bounds=reader->bounds();
    int il1 = std::max( bounds.i1(), m_il1);
    int il2 = std::min( bounds.i2(), m_il2);
    int xl1 = std::max( bounds.j1(), m_xl1);
    int xl2 = std::min( bounds.j2(), m_xl2);
    auto k1=std::max( bounds.timeToSample( 0.001*m_msec1 ), 0 );
    auto k2=std::min( bounds.timeToSample( 0.001*m_msec2 ), bounds.nt() );

    seismic::Trace trace(bounds.sampleToTime(k1), bounds.dt(), k2-k1+1 );
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();

    thdr["delrt"]=seismic::HeaderValue::makeIntValue(1000000*bounds.sampleToTime(k1));
    thdr["ns"]=seismic::HeaderValue::makeUIntValue(k2-k1+1);
    thdr["dt"]=seismic::HeaderValue::makeUIntValue(1000000*bounds.dt());	// !!!us
    thdr["scalco"]=seismic::HeaderValue::makeIntValue(SCALCO);
    thdr["offset"]=seismic::HeaderValue::makeFloatValue(0); // MUST BE HERE: this is used for creating index file

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !project()->geometry().computeTransforms( XYToIlXl, IlXlToXY)){

        setErrorString("Invalid geometry, failed to compute transformations!");
        return ResultCode::Error;
    }

    // ADJUST DOMAIN later
    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               3, SeismicDatasetInfo::Domain::Time, SeismicDatasetInfo::Mode::Poststack,
                                                               bounds.sampleToTime(k1), bounds.dt(), k2-k1+1);

    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );
    if( !writer){
        setErrorString("Creating dataset writer failed!");
        return ResultCode::Error;
    }

    const int CHUNK_ILINES=10;

    emit started(bounds.ni());
    qApp->processEvents();

    int n=0;
    for( int il1=bounds.i1(); il1<bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, bounds.i2());
        auto subvolume=reader->readIl(il1, il2);
        if( !subvolume){
            setErrorString("Reading subvolume failed!");
            return ResultCode::Error;
        }

        // process chunk
        auto sbounds=subvolume->bounds();
        for( int i=sbounds.i1(); i<=sbounds.i2(); i++){

            for( int j=sbounds.j1(); j<=sbounds.j2(); j++){

                ++n;

                QPointF p=IlXlToXY.map( QPoint(i,j));
                qreal x=p.x();
                qreal y=p.y();
                int cdp=1 + (i-bounds.i1())*bounds.nj() + j - bounds.j1();

                thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);                                        // XXX, maybe read from db once
                thdr["tracf"]=seismic::HeaderValue::makeIntValue(n);
                thdr["iline"]=seismic::HeaderValue::makeIntValue(i);
                thdr["xline"]=seismic::HeaderValue::makeIntValue(j);
                thdr["sx"]=seismic::HeaderValue::makeFloatValue(x);
                thdr["sy"]=seismic::HeaderValue::makeFloatValue(y);
                thdr["gx"]=seismic::HeaderValue::makeFloatValue(x);
                thdr["gy"]=seismic::HeaderValue::makeFloatValue(y);
                thdr["cdpx"]=seismic::HeaderValue::makeFloatValue(x);
                thdr["cdpy"]=seismic::HeaderValue::makeFloatValue(y);

                for( int k=k1; k<=k2; k++){
                    samples[k-k1]=(*subvolume)(i,j,k);
                }

                writer->writeTrace( trace );

            }

        }   // finished chunk

        emit progress(il2-bounds.i1());
        qApp->processEvents();
    }

    reader->close();
    writer->close();

    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
