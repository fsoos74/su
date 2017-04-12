#include "frequencyspectra.h"

#include <fft.h>
#include <vector>

Spectrum computeSpectrum( const seismic::Trace& trc, double window_start, double window_len){

    if( window_start < trc.ft() ) throw std::runtime_error("INVALID WINDOW START");

    // translate window fro seconds to samples
    seismic::Trace::size_type i1 = trc.time_to_index(window_start);
    seismic::Trace::size_type i2 = trc.time_to_index(window_start + window_len);
    seismic::Trace::size_type ni = i2 - i1 +1;      // window len in samples

    // find fft window len, must be power of 2
    seismic::Trace::size_type fftlen = 1;
    while( fftlen < ni ) fftlen *= 2;

    // fill input buffer for fft
    std::vector<seismic::Trace::sample_type> ibuf(fftlen);
    std::copy(trc.samples().begin()+i1, trc.samples().begin()+i2, ibuf.begin());

    // ADD additional code here for window len not power of 2
    //
    //

    // fft
    auto obuf=fft(ibuf);

    // compute frequency step
    auto df = 1. / ( fftlen * trc.dt() );

    return Spectrum( 0, df, obuf.begin(), obuf.end() );
}

/*
QVector<QPointF> generateSpectrum( const seismic::Trace& trace, size_t index, HorizonWindowPosition pos, size_t window_size ){

    std::vector<float> ibuf(window_size);
    auto freqs = fft_freqs(trace.dt(), window_size);
    ssize_t windowIndex=index;

    switch(pos){
    case HorizonWindowPosition::Above: windowIndex-=window_size; break;
    case HorizonWindowPosition::Center: windowIndex-=window_size/2; break;
    case HorizonWindowPosition::Below: break;
    }

    if( windowIndex<0 || windowIndex+window_size>=trace.samples().size() ) return QVector<QPointF>();

    seismic::Trace::Samples::const_iterator begin=trace.samples().cbegin() + windowIndex;
    seismic::Trace::Samples::const_iterator end=trace.samples().cbegin() + windowIndex + window_size + 1;

    std::copy( begin, end, ibuf.begin() );
    auto obuf=fft(ibuf);

    QVector<QPointF> res;
    res.reserve(obuf.size());

    for( int i=0; i<obuf.size(); i++){
       res.push_back( QPointF(freqs[i], abs(obuf[i]) ) );
    }

    return res;
}
*/
