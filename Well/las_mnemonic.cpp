#include "las_mnemonic.h"

namespace well{

LASMnemonic::LASMnemonic( const std::string& name,
        const std::string& unit,
        const std::string& data,
        const std::string& description):
        name(name), unit(unit),data(data),description(description){

}

}
