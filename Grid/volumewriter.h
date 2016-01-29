#ifndef VOLUMEWRITER_H
#define VOLUMEWRITER_H


#include<grid3d.h>
#include<iostream>
#include<fstream>
#include<QString>


class VolumeWriter
{
public:

    VolumeWriter(const Grid3D<float>&);

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
   bool writeData(std::ofstream&);
   void setError(const QString& msg);

   const  Grid3D<float>& m_volume;
   bool m_error=false;
   QString m_errorString;
};

#endif // VOLUMEWRITER_H
