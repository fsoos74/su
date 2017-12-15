#include "stringio.h"

#include<vector>

void writeString( std::ostream& os, const std::string& str){
    size_t len = str.size();
    os.write((char*) &len, sizeof(size_t));
    os.write(str.c_str(), len);
}

std::string readString(std::istream& istr){
    size_t len=0;
    istr.read((char*)&len, sizeof(size_t));
    std::vector<char> tmp(len+1);
    istr.read(&tmp[0], len);
    tmp[len] = '\0';
    return std::string(&tmp[0]);
}
