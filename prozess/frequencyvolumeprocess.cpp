#include "frequencyvolumeprocess.h"

#include<fft.h>
#include<frequencyspectra.h>
#include<spectrum.h>

#include <omp.h>


FrequencyVolumeProcess::FrequencyVolumeProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Frequency Volume"), project, parent){
}



ProjectProcess::ResultCode FrequencyVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputName;
    QString outputName;
    try{
        outputName=getParam(parameters, "output-volume");
        inputName=getParam(parameters, "input-volume");
        m_minimumFrequency=getParam( parameters, "minimum-frequency").toDouble();
        m_maximumFrequency=getParam( parameters, "maximum-frequency").toDouble();
        m_windowSamples=getParam( parameters, "window-samples").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }
    ProjectProcess::ResultCode res=addInputVolume(inputName);
    if( res!=ResultCode::Ok) return res;
    auto ibounds=inputBounds(0);
    setBounds(ibounds);
    res=addOutputVolume(outputName, bounds(), inputDomain(0), VolumeType::Other);
    if( res!=ResultCode::Ok) return res;
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


ProjectProcess::ResultCode FrequencyVolumeProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){
    auto output=outputs[0];
    auto input=inputs[0];
    auto freqs = fft_freqs(bounds().dt(), m_windowSamples);
    int k1=static_cast<int>(m_windowSamples/2);
    int k2=bounds().nt()-k1;

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        #pragma omp parallel for
        for( int k=k1; k<k2; k++){

            auto spectrum=computeSpectrum( &((*input)(iline,j,k)), m_windowSamples, bounds().dt() );
            auto all=integratedPower(spectrum, 0, 1000);
            if(all<=0) continue;
            auto area=integratedPower(spectrum, m_minimumFrequency, m_maximumFrequency );
            (*output)(iline, j, k)=area/all;
        }

    }

    return ResultCode::Ok;
}


