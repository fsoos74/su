#ifndef SEISMIC_H
#define SEISMIC_H

#include "seismic_global.h"

#include<memory>

namespace seismic{

class Trace;
class Gather;

}

std::shared_ptr<seismic::Trace> SEISMICSHARED_EXPORT agc(const seismic::Trace& in, size_t len);
std::shared_ptr<seismic::Gather> SEISMICSHARED_EXPORT agc(const seismic::Gather& in, size_t len);

class SEISMICSHARED_EXPORT Seismic
{

public:
    Seismic();
};




#endif // SEISMIC_H
