#ifndef SEGYFORMATERROR_H
#define SEGYFORMATERROR_H

#include<stdexcept>

namespace seismic{


struct SEGYFormatError:public std::runtime_error{
    SEGYFormatError( const std::string& msg);
};


}

#endif // SEGYFORMATERROR_H
