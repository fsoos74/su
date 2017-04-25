#ifndef FREQUENCYSPECTRA_H
#define FREQUENCYSPECTRA_H

#include <trace.h>
#include <spectrum.h>

Spectrum computeSpectrum( const seismic::Trace&, size_t window_center, size_t windowSamples);
Spectrum::float_type integratedPower( const Spectrum&, const Spectrum::float_type& f1, const Spectrum::float_type& f2);


#endif // FREQUENCYSPECTRA_H
