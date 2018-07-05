#include "cropdatasetprocess.h"

#include <avoproject.h>
#include <processparams.h>
#include "utilities.h"
#include <trace.h>
#include<iostream>

CropDatasetProcess::CropDatasetProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Crop Dataset"), project, parent){

}

ProjectProcess::ResultCode CropDatasetProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
       m_outputName=getParam( parameters, QString("output-dataset") );
       m_inputName=getParam( parameters, QString("input-dataset") );
       m_minIline=getParam( parameters, "min-iline").toInt();
       m_maxIline=getParam( parameters, "max-iline").toInt();
       m_minXline=getParam( parameters, "min-xline").toInt();
       m_maxXline=getParam( parameters, "max-xline").toInt();
       m_minMSec=getParam( parameters, "min-msec").toInt();
       m_maxMSec=getParam( parameters, "max-msec").toInt();
       m_minOffset=getParam( parameters, QString("min-offset") ).toFloat();
       m_maxOffset=getParam( parameters, QString("max-offset") ).toFloat();
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


ProjectProcess::ResultCode CropDatasetProcess::run(){

    int odtms = 1000 * m_reader->info().dt();
    int ons = (m_maxMSec - m_minMSec) / odtms + 1;

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                               m_reader->info().dimensions(), m_reader->info().domain(), m_reader->info().mode(),
                                                               0.001*m_minMSec, m_reader->info().dt(), static_cast<size_t>(ons));


    m_reader->seekTrace(0);
    m_writer = std::make_shared<SeismicDatasetWriter>( dsinfo );

    emit started(m_reader->sizeTraces() );
    qApp->processEvents();

    while( m_reader->good() ){

        seismic::Trace trace=m_reader->readTrace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();
        float offset=header.at("offset").floatValue();

        if( iline>=m_minIline && iline<=m_maxIline
                && xline>=m_minXline && xline<=m_maxXline
                && offset>=m_minOffset && offset<=m_maxOffset
                ){

            auto sam=trace.samples();
            seismic::Trace::Samples osam(ons);
            auto ifirst=trace.time_to_index(0.001*m_minMSec);
            auto ilast=trace.time_to_index(0.001*m_maxMSec);
            std::copy(&sam[ifirst], &sam[ilast],&osam[0]);
            seismic::Trace otrace(0.001*m_minMSec, 0.001*odtms, osam, header);

            m_writer->writeTrace(otrace);
        }

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
