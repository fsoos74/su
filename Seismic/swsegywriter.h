#ifndef SWSEGYWRITER_H
#define SWSEGYWRITER_H

#include "segywriter.h"

namespace seismic{


class SWSEGYWriter : public SEGYWriter
{
public:

    SWSEGYWriter( const std::string& name,
                const SEGYInfo& info,
                const SEGYTextHeader& textHeader,   //only support 1 ebcdic/textual header block for now
                const Header& binaryHeader,
                  const std::string& lineName="line1234");


protected:
    virtual void process_raw_binary_header( std::vector<char>&)override;

private:
    std::string m_lineName;
};

}

#endif // SWSEGYWRITER_H
