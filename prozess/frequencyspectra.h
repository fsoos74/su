#ifndef FREQUENCYSPECTRA_H
#define FREQUENCYSPECTRA_H

#include <trace.h>
#include <spectrum.h>

Spectrum computeSpectrum( const seismic::Trace&, double window_start, double window_len);

#endif // FREQUENCYSPECTRA_H
