#include "hilbert.h"

#include "fft.h"


// hilbert transform using fft, in size must be power of 2 !!!
std::vector<float> hbt( std::vector<float>& in ){

    auto X = fft( in );

// no negative freq because input was real
// multiply positive freqs by -i
    X[0]=0;
    for( size_t i = 1; i<=X.size()/2; i++){
        X[i]*=std::complex<float>(0,-1);
    }

    return ifft( X );
}

std::vector<std::complex<float>> hilbert_trace(std::vector<float> real_trc ){

    size_t n=1;
    while( n<=real_trc.size() ) n<<=1;
    std::vector<float> in(n,0);
    std::copy( real_trc.begin(), real_trc.end(), in.begin() );
    auto imag_trc= hbt(in);
    std::vector<std::complex<float>> complex_trc;
    complex_trc.reserve(real_trc.size());
    for( auto itr=real_trc.begin(), iti=imag_trc.begin(); itr!=real_trc.end(); ++itr, ++iti)
        complex_trc.push_back( std::complex<float>(*itr,*iti) );
    return complex_trc;
}
