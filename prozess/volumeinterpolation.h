#ifndef VOLUMEINTERPOLATION_H
#define VOLUMEINTERPOLATION_H

#include <volume.h>

// compute intermediate values for a volume that has values on w_lines/w_samples intervals
void interpolateIntermediate(Volume& v, int w_lines, int w_samples);



#endif // VOLUMEINTERPOLATION_H
