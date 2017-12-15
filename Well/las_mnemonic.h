#ifndef LAS_MNEMONIC_H
#define LAS_MNEMONIC_H

#include<string>

namespace well{


struct LASMnemonic{

    LASMnemonic(){}

    LASMnemonic( const std::string& name,
        const std::string& unit,
        const std::string& data,
        const std::string& description);

    std::string name;
    std::string unit;
    std::string data;
    std::string description;
};

}

#endif // MNEMONIC_H
