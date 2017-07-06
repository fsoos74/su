#include "frequencyvolumeprocess.h"

#include "utilities.h"      // setRequiredHeaderwords

#include<seismicdatasetreader.h>
#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>
#include<trace.h>
#include<fft.h>
#include<frequencyspectra.h>
#include<spectrum.h>
#include<future>

#include <omp.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "dt", "ns" };

FrequencyVolumeProcess::FrequencyVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Frequency Volume"), project, parent){

}

ProjectProcess::ResultCode FrequencyVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("input-volume"))){

        setErrorString("Parameters contain no input-volume!");
        return ResultCode::Error;
    }
    QString inputName=parameters.value( QString("input-volume") );
    m_inputVolume=project()->loadVolume( inputName );
    if( !m_inputVolume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }
    m_bounds=m_inputVolume->bounds();

    if( !parameters.contains(QString("output-volume"))){
        setErrorString("Parameters contain no output volume name!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("output-volume"));

    m_volume=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    if( !parameters.contains(QString("minimum-frequency"))){

        setErrorString("Parameters don't contain minimum-frequency");
        return ResultCode::Error;
    }
    m_minimumFrequency=parameters.value("minimum-frequency").toDouble();

    if( !parameters.contains(QString("maximum-frequency"))){

        setErrorString("Parameters don't contain maximum-frequency");
        return ResultCode::Error;
    }
    m_maximumFrequency=parameters.value("maximum-frequency").toDouble();

    if( !parameters.contains(QString("window-samples"))){

        setErrorString("Parameters don't contain window-samples");
        return ResultCode::Error;
    }
    m_windowSamples=parameters.value("window-samples").toInt();


    return ResultCode::Ok;
}




template< typename CONST_ITERATOR >
float frequencyAmplitude( CONST_ITERATOR first, CONST_ITERATOR last, const std::vector<float>& freqs,
                          float minimumFrequency, float maximumFrequency ){
    std::vector<float> ibuf( first, last);
    // add check that ibuf.size is power of 2  and check freqs.size>=ibuf.size!!!
    auto obuf=fft(ibuf);

    float mv = 0;
    for( int i=0; i<obuf.size(); i++){

        float v = std::norm( obuf[i] );    // complex abs squared

        if( minimumFrequency<=freqs[i] && freqs[i]<=maximumFrequency) mv+=v;
    }

    return mv;

}



ProjectProcess::ResultCode FrequencyVolumeProcess::run(){

    auto bounds=m_volume->bounds();

    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero

    auto freqs = fft_freqs(bounds.dt(), m_windowSamples);

    emit currentTask("Computing output volume");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.inline1()+1; i<=bounds.inline2()-1; i++){

        for( int j=bounds.crossline1()+1; j<=bounds.crossline2()-1; j++){

            #pragma omp parallel for
            for( int k=m_windowSamples/2; k<m_bounds.sampleCount()-m_windowSamples/2; k++){

                auto spectrum=computeSpectrum( &((*m_inputVolume)(i,j,k)), m_windowSamples, bounds.dt() );
                auto all=integratedPower(spectrum, 0, 1000);
                auto area=integratedPower(spectrum, m_minimumFrequency, m_maximumFrequency );
                (*m_volume)(i, j, k)= area/all;

            }

        }

        if( (i-bounds.inline1()) % onePercent==0 ){
            emit progress((i-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addVolume( m_volumeName, m_volume ) ) {
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    return ResultCode::Ok;
}


