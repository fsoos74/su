#ifndef STRINGIO_H
#define STRINGIO_H


#include<iostream>
#include<fstream>
#include<string>

void writeString( std::ostream& os, const std::string& str);

std::string readString(std::istream& istr);

#endif // STRINGIO_H
