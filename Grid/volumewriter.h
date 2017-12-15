#ifndef VOLUMEWRITER_H
#define VOLUMEWRITER_H


#include<grid3d.h>
#include<iostream>
#include<fstream>
#include<QString>

#include<volume.h>

class VolumeWriter
{
public:

    VolumeWriter(const Volume&);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool writeToStream(std::ofstream&);

private:

   bool writeMagic(std::ofstream&);
   bool writeBounds(std::ofstream&);
   bool writeDomainAndType(std::ofstream&);
   bool writeData(std::ofstream&);
   void setError(const QString& msg);

   const  Volume& m_volume;
   bool m_error=false;
   QString m_errorString;
};

#endif // VOLUMEWRITER_H
