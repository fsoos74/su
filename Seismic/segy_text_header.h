#ifndef SEGY_TEXT_HEADER_H
#define SEGY_TEXT_HEADER_H

#include<array>
#include<vector>
#include<string>

namespace seismic{

    const size_t SEGY_TEXT_HEADER_SIZE=3200;
    typedef std::array<char, SEGY_TEXT_HEADER_SIZE> SEGYTextHeader;
    typedef std::vector<std::string> SEGYTextHeaderStr;         // string representation
    SEGYTextHeaderStr convertToString( const SEGYTextHeader& thdr, bool conv_ebcdic=true, const size_t nrows=40, const size_t ncols=80);
    SEGYTextHeader convertToRaw( const SEGYTextHeaderStr& str, bool conv_ebcdic=true, const size_t nrows=40, const size_t ncols=80);
}

#endif // SEGY_TEXT_HEADER_H

