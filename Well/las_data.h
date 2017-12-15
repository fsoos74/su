#ifndef LAS_DATA_H
#define LAS_DATA_H

#include<unordered_map>
#include<vector>
#include<list>

#include "las_mnemonic.h"

namespace well{

struct LASData{

    std::unordered_map<std::string, LASMnemonic> versionInformation;
    std::unordered_map<std::string, LASMnemonic> wellInformation;
    std::unordered_map<std::string, LASMnemonic> parameterInformation;
    std::vector<LASMnemonic> curveInformation;         // retain order
    std::list<std::string> otherInformation;        // no mnemonic but plain text
    std::vector< std::vector<double> > curves;

    size_t curveCount()const;

    const std::vector<double>& curveData( size_t i )const;

    LASMnemonic curveInfo( size_t i )const;
};

}

#endif // DATA_H
