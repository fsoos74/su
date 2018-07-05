#include "createtimesliceprocess.h"

#include<seismicdatasetinfo.h>
#include<chrono>
#include<iostream>

#include "utilities.h"

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns" };

CreateTimesliceProcess::CreateTimesliceProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Create Timeslice"), project, parent){

}

ProjectProcess::ResultCode CreateTimesliceProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString horizonName;
    QString inputName;

    try{
        inputName=getParam(parameters, "input");
        m_sliceName=getParam(parameters, "slice");
        m_useHorizon=static_cast<bool>(getParam(parameters, "use-horizon").toInt());
        horizonName=getParam(parameters, "horizon");
        m_sliceTime = parameters.value(QString("time")).toDouble();

    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    m_reader= project()->openSeismicDataset(inputName);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(inputName) );
        return ResultCode::Error;
    }

    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }

    auto vbounds=Grid2DBounds(m_reader->minInline(), m_reader->minCrossline(),
            m_reader->maxInline(), m_reader->maxCrossline() );
    Grid2DBounds bounds;

    if( m_useHorizon){
        m_horizon=project()->loadGrid( GridType::Horizon, horizonName);
        if( !m_horizon ){
            setErrorString("Loading horizon failed!");
            return ResultCode::Error;
        }
        auto hbounds=m_horizon->bounds();
        // find area covered by volume and horizon
        bounds=Grid2DBounds( std::max(hbounds.i1(),vbounds.i1()), std::min(hbounds.i2(),vbounds.i2()),
                                  std::max(hbounds.j1(),vbounds.j1()), std::min(hbounds.j2(),vbounds.j2()));

    }else{
        bounds=vbounds;
    }

    m_slice=std::make_shared<Grid2D<float> >( Grid2DBounds(bounds.i1(), bounds.j1(), bounds.i2(), bounds.j2()) );

    if( !m_slice ){
        setErrorString("Allocating slice failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CreateTimesliceProcess::run(){

    emit started(m_reader->sizeTraces());
    m_reader->seekTrace(0);

    while( m_reader->good() ){

        seismic::Trace trace=m_reader->readTrace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        qreal t;

        if( m_useHorizon){ // we are using a horizon and not constant time

            //if( !m_horizon->bounds().isInside(iline, xline)) continue; // don't need this anymore. valueAt does the job
            Grid2D<float>::value_type v=m_horizon->valueAt(iline, xline);
            if( v == m_horizon->NULL_VALUE ) continue;

            t=0.001 * v;    // horizon in millis
        }
        else{
            t=0.001*m_sliceTime;  // slicetime in millisecs
        }

        // interpolate between nearest samples, should add this to trace
        qreal x=(t - trace.ft() )/trace.dt();
        size_t i=std::truncf(x);
        x-=i;
        if( x>=0 && i<trace.samples().size()){
            (*m_slice)(iline, xline)=( 1.-x) * trace.samples()[i] + x*trace.samples()[i+1];
        }

        emit progress( m_reader->tellTrace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


    std::pair<GridType, QString> sliceTypeAndName = splitFullGridName( m_sliceName );
    if( !project()->addGrid( sliceTypeAndName.first, sliceTypeAndName.second, m_slice, params())){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_sliceName) );
        return ResultCode::Error;
    }
    emit finished();

    return ResultCode::Ok;
}

