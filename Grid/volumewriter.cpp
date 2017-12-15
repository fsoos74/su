#include "volumewriter.h"

VolumeWriter::VolumeWriter(const Volume& v):m_volume(v),m_error(false){

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

    if(!writeDomainAndType(os)){
        setError("Writing domain and type failed!");
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

    const size_t VERSION2_MAGIC_LEN=9;
    char VERSION2_MAGIC[]="version2";

    os.write(VERSION2_MAGIC, VERSION2_MAGIC_LEN );

    return os.good();
}

bool VolumeWriter::writeBounds(std::ofstream& os){

    const Grid3DBounds& bounds=m_volume.bounds();

    qint64 il1=bounds.i1();
    os.write((char*)&il1,sizeof(il1));
    qint64 il2=bounds.i2();
    os.write((char*)&il2,sizeof(il2));

    qint64 xl1=bounds.j1();
    os.write((char*)&xl1,sizeof(xl1));
    qint64 xl2=bounds.j2();
    os.write((char*)&xl2,sizeof(xl2));

    qreal ft=bounds.ft();
    os.write((char*)&ft, sizeof(ft));
    qreal dt=bounds.dt();
    os.write((char*)&dt, sizeof(dt));
    qint64 sampleCount=bounds.nt();
    os.write((char*)&sampleCount,sizeof(sampleCount));

    return os.good();
}

bool VolumeWriter::writeDomainAndType(std::ofstream & os){

    qint16 dom=static_cast<qint16>(m_volume.domain());
    os.write((char*)&dom, sizeof(dom));

    qint16 type=static_cast<qint16>(m_volume.type());
    os.write((char*)&type, sizeof(type));

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

