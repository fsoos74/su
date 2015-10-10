#ifndef HEADER_H
#define HEADER_H

#include<string>
#include<unordered_map>
#include<headervalue.h>

namespace seismic {
    typedef std::unordered_map<std::string, HeaderValue> Header;
}



#endif // HEADER_H

