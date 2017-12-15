#ifndef WELLPATHWRITER_H
#define WELLPATHWRITER_H

#include<wellpath.h>
#include<iostream>
#include<fstream>
#include<QString>


class WellPathWriter
{
public:

    WellPathWriter(const WellPath&);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool writeToStream(std::ofstream&);

private:

   bool writeMagic(std::ofstream&);
   bool writeHeader(std::ofstream&);
   bool writeData(std::ofstream&);
   void setError(const QString& msg);

   const  WellPath& m_path;
   bool m_error=false;
   QString m_errorString;
};

#endif // WELLPATHWRITER_H
