#ifndef FFT_H
#define FFT_H

#include<vector>
#include<complex>

// fft of vector of real values
std::vector< std::complex<float> > fft( const std::vector<float>&);

// inverse fft from fft of real values
std::vector<float> ifft( const std::vector< std::complex<float> >&);

// positive frequencies of fft of N real values with sampling inverval dt
std::vector<float> fft_freqs( const float& dt, unsigned N );

#endif // FFT_H
