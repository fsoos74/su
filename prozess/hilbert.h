#ifndef HILBERT_H
#define HILBERT_H

#include<vector>
#include<complex>

// hilbert transform using fft, in size must be power of 2 !!!
std::vector<float> hbt( std::vector<float>& in );

// analytical trace for instantaneous attributes, complex, real is input trace, imaginary is hilbert transformed input
// input length must not be power of 2
std::vector<std::complex<float>> hilbert_trace(std::vector<float> real_trc );

#endif // HILBERT_H
