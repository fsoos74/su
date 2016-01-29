#include "volumewriter.h"

VolumeWriter::VolumeWriter(const Grid3D<float>& v):m_volume(v),m_error(false){

}


bool VolumeWriter::writeToStream(std::ofstream& os){

    if(!writeMagic(os)){
        setError("Writing file id failed!");
        return false;
    }

    if(!writeBounds(os)){
        setError("Writing boundaries failed!");
        return false;
    }

    if(!writeData(os)){
        setError("Writing data failed!");
        return false;
    }

    return true;
}

bool VolumeWriter::writeMagic(std::ofstream& os){

    const size_t MAGIC_LEN=19;
    char MAGIC[]="Binary Volume File";

    os.write(MAGIC, MAGIC_LEN);

    return os.good();
}

bool VolumeWriter::writeBounds(std::ofstream& os){

    const Grid3DBounds& bounds=m_volume.bounds();

    qint64 il1=bounds.inline1();
    os.write((char*)&il1,sizeof(il1));
    qint64 il2=bounds.inline2();
    os.write((char*)&il2,sizeof(il2));

    qint64 xl1=bounds.crossline1();
    os.write((char*)&xl1,sizeof(xl1));
    qint64 xl2=bounds.crossline2();
    os.write((char*)&xl2,sizeof(xl2));

    qreal ft=bounds.ft();
    os.write((char*)&ft, sizeof(ft));
    qreal dt=bounds.dt();
    os.write((char*)&dt, sizeof(dt));
    qint64 sampleCount=bounds.sampleCount();
    os.write((char*)&sampleCount,sizeof(sampleCount));

    return os.good();
}

bool VolumeWriter::writeData(std::ofstream& os){

    os.write( (char*)&(m_volume.values()[0]), m_volume.values().size()*sizeof(float) );
    return os.good();

}

void VolumeWriter::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}

