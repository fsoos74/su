#include "volumereader.h"

#include<cstring>

VolumeReader::VolumeReader(Grid3D<float>& v):m_volume(v){

}

bool VolumeReader::readFromStream(std::ifstream& istr){

    if( !readMagic(istr)){
        return false;
    }

    Grid3DBounds bounds;
    if(!readBounds(istr, bounds)){
        return false;
    }

    Grid3D<float> volume(bounds);
    if(!readData(istr,volume)){
        return false;
    }

    m_volume=volume;

    return true;
}


bool VolumeReader::readMagic(std::ifstream& istr){

    const size_t MAGIC_LEN=19;
    char MAGIC[]="Binary Volume File";
    char magic[MAGIC_LEN];

    istr.read(magic, MAGIC_LEN);
    if( !istr.good()){
        setError("Reading file id failed!");
        return false;
    }

    if(strncmp(magic, MAGIC, MAGIC_LEN!=0)){
        setError("Wrong file id!");
        return false;
    }

    return true;
}


bool VolumeReader::readBounds(std::ifstream& istr, Grid3DBounds& bounds){

    qint64 il1;
    istr.read((char*)&il1, sizeof(il1));
    qint64 il2;
    istr.read((char*)&il2, sizeof(il2));

    qint64 xl1;
    istr.read((char*)&xl1, sizeof(xl1));
    qint64 xl2;
    istr.read((char*)&xl2, sizeof(xl2));

    qreal ft;
    istr.read((char*)&ft, sizeof(ft));
    qreal dt;
    istr.read((char*)&dt, sizeof(dt));
    qint64 sampleCount;
    istr.read((char*)&sampleCount, sizeof(sampleCount));

    if(!istr.good()){
        setError("Reading boundaries failed!");
        return false;
    }

    bounds=Grid3DBounds(il1,il2, xl1,xl2, sampleCount, ft, dt);
    return true;
}

bool VolumeReader::readData(std::ifstream& istr, Grid3D<float>& volume){

    istr.read( (char*)&(volume.values()[0]), volume.values().size()*sizeof(float));
    if(!istr.good()){
        setError("Reading volume data failed!");
        return false;
    }

    return true;
}


void VolumeReader::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}
