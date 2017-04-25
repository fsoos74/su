#include "frequencyspectra.h"

#include <fft.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>

using namespace std::placeholders;

auto hanning=[](int n, int N){
    return 0.5*(1.-cos(2*M_PI*n/(N-1)));
};

Spectrum computeSpectrum( const seismic::Trace& trc, size_t window_center, size_t window_len){

    if( window_center <  window_len/2 || window_center+window_len/2>=trc.size() ) return Spectrum();    // return NULL spectrum is window out of trace

    size_t window_start=window_center-window_len/2;

    // find fft window len, must be power of 2
    seismic::Trace::size_type fftlen = 1;
    while( fftlen < window_len ) fftlen *= 2;

    // fill input buffer for fft, apply window taper function
    std::vector<seismic::Trace::sample_type> ibuf(fftlen);    
    std::copy( trc.samples().begin()+window_start, trc.samples().begin()+window_start+window_len+1, ibuf.begin() );
    //apply window function
    auto wf=std::bind( hanning, _1, window_len);
    for( int i=0; i<window_len; i++){
        ibuf[i]*=wf(i);
    }

    // ADD additional code here for window len not power of 2 (e.g. padding)
    //
    //

    // fft
    auto obuf=fft(ibuf);

    // compute frequency step
    auto df = 1. / ( fftlen * trc.dt() );

    return Spectrum( 0, df, obuf.begin(), obuf.end() );
}

/* ORIGINAL
Spectrum computeSpectrum( const seismic::Trace& trc, size_t window_center, size_t window_len){

    if( window_center <  window_len/2 || window_center+window_len/2>=trc.size() ) return Spectrum();    // return NULL spectrum is window out of trace

    size_t window_start=window_center-window_len/2;

    // find fft window len, must be power of 2
    seismic::Trace::size_type fftlen = 1;
    while( fftlen < window_len ) fftlen *= 2;

    // fill input buffer for fft
    std::vector<seismic::Trace::sample_type> ibuf(fftlen);
    std::copy(trc.samples().begin()+window_start, trc.samples().begin()+window_start+window_len+1, ibuf.begin());

    // ADD additional code here for window len not power of 2 (e.g. padding)
    //
    //

    // apply window function
    for( auto i=0; i<ibuf.size(); i++){
        //ibuf[i]*=sin( M_PI*i/(window_len-1));   // cosine
        ibuf[i]*=0.5*(1.-cos(2*M_PI*i/(window_len-1)));  // hanning
    }


    // fft
    auto obuf=fft(ibuf);

    // compute frequency step
    auto df = 1. / ( fftlen * trc.dt() );

    return Spectrum( 0, df, obuf.begin(), obuf.end() );
}
*/

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

// this works because power used fabs(amplitude), negative y-values are not handled by this integration algorithm
Spectrum::float_type integratedPower( const Spectrum& spectrum, const Spectrum::float_type& f1, const Spectrum::float_type& f2){

    if( f1>=f2 ) return 0;  // invalid or empty range
    if( f2<=spectrum.fmin() ) return 0;    // nothing of spectrum within range

    Spectrum::float_type sum=0;

    for( Spectrum::index_type i = 1; i<spectrum.size(); i++ ){

        auto fleft=spectrum.frequency(i-1);
        auto aleft=std::fabs(spectrum.amplitude(i-1));
        auto fright=spectrum.frequency(i);
        auto aright=std::fabs(spectrum.amplitude(i));

        if( fright<=f1 ) continue;

        auto fl=std::max( f1, fleft );
        if( fl>=fright ) break;
        auto fr=std::min( f2, fright );

        auto al=aleft+( fl-fleft)*( aright-aleft)/(fright-fleft);
        auto ar=aleft+( fr-fleft)*( aright-aleft)/(fright-fleft);
if( fr<fl)continue; // TESTZ
        auto mn = std::min(al,ar);
        sum += mn*(fr-fl) + 0.5*std::fabs(ar-al)*(fr-fl);
    }
    return sum;
}

