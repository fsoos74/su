#include "extractdatasetprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
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

    m_inputVolume=project()->loadVolume( m_inputName);
    if( !m_inputVolume ){
        setErrorString("Loading input volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


#include<iostream>

ProjectProcess::ResultCode ExtractDatasetProcess::run(){

    const int SCALCO=-10;

    Grid3DBounds bounds=m_inputVolume->bounds();
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
    thdr["dt"]=seismic::HeaderValue::makeUIntValue(1000*bounds.dt());
    thdr["scalco"]=seismic::HeaderValue::makeIntValue(SCALCO);
    thdr["offset"]=seismic::HeaderValue::makeFloatValue(0); // MUST BE HERE: this is used for creating index file

    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( !project()->geometry().computeTransforms( XYToIlXl, IlXlToXY)){

        setErrorString("Invalid geometry, failed to compute transformations!");
        return ResultCode::Error;
    }

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               3, SeismicDatasetInfo::Domain::Time, SeismicDatasetInfo::Mode::Poststack,
                                                               bounds.sampleToTime(k1), bounds.dt(), k2-k1+1);

    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );

    emit currentTask("Processing...");
    emit started(il2-il1+1);
    qApp->processEvents();

    int n=0;

    for( int il=il1; il<=il2; il++){

        for( int xl=xl1; xl<=xl2; xl++){

            ++n;

            QPointF p=IlXlToXY.map( QPoint(il, xl));
            qreal x=p.x();
            qreal y=p.y();
//std::cout<<"n="<<n<<" il="<<il<<" xl="<<xl<<" x="<<x<<" y="<<y<<std::endl<<std::flush;

            int cdp=1 + (il-bounds.i1())*bounds.nj() + xl - bounds.j1();

            thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);                                        // XXX, maybe read from db once
            thdr["tracf"]=seismic::HeaderValue::makeIntValue(n);
            thdr["iline"]=seismic::HeaderValue::makeIntValue(il);
            thdr["xline"]=seismic::HeaderValue::makeIntValue(xl);
            thdr["sx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["sy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["gx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["gy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["cdpx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["cdpy"]=seismic::HeaderValue::makeFloatValue(y);

            for( int k=k1; k<=k2; k++){
                samples[k-k1]=(*m_inputVolume)(il,xl,k);
            }

            writer->writeTrace( trace );

        }

        emit progress(il-il1);
        qApp->processEvents();
    }


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
