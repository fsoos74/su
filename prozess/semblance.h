#ifndef SEMBLANCE_H
#define SEMBLANCE_H

#include<gather.h>
#include<grid3d.h>
#include<volumediptracker.h>

double semblance( const seismic::Gather& g, size_t halfSamples, double tt);
double semblance( const Grid3D<float>& vol, const std::vector<std::pair<int,int>>& locations, int halfSamples, int k);
double semblance( const Grid3D<float>& vol, const std::vector<std::tuple<int,int,double>>& path, int halfSamples);


#endif // SEMBLANCE_H
