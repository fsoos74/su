#ifndef AGC_H
#define AGC_H

#include<trace.h>
#include<gather.h>
#include<memory>


namespace seismic{

std::shared_ptr<seismic::Trace> agc(const seismic::Trace& in, size_t len);
std::shared_ptr<seismic::Gather> agc(const seismic::Gather& in, size_t len);

};

#endif // AGC_H
