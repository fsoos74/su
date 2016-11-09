#include "segyformaterror.h"

namespace seismic{


SEGYFormatError::SEGYFormatError( const std::string& msg ):
    std::runtime_error(msg)
{

}


}
