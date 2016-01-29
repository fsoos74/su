#ifndef VOLUMEREADER_H
#define VOLUMEREADER_H

#include<memory>

#include<fstream>
#include<memory>
#include<grid3d.h>
#include<QString>

class VolumeReader
{
public:

    VolumeReader(Grid3D<float>& );

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool readFromStream(std::ifstream&);

private:

    bool readMagic(std::ifstream&);
    bool readBounds(std::ifstream&,Grid3DBounds&);
    bool readData(std::ifstream&,Grid3D<float>&);
    void setError(const QString& msg);

    Grid3D<float>& m_volume;
    bool m_error=false;
    QString m_errorString;
};

#endif // VOLUMEREADER_H
