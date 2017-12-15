#include "volumereader.h"

#include<cstring>
#include<QApplication>  // qApp
#include<iostream>

VolumeReader::VolumeReader(Volume* v, QObject* parent ):QObject(parent),m_volume(v){

}

bool VolumeReader::readFromStream(std::ifstream& istr){

    if( !readMagic(istr)){
        return false;
    }

    if(!readBounds(istr, m_bounds )){
        return false;
    }

    if(m_version==2){
        if(!readDomainAndType(istr, m_domain, m_type)) return false;
    }

    if( !m_volume ) return true;

    Volume volume(m_bounds, m_domain, m_type);

    if(!readData(istr, &volume[0], volume.size())){
        return false;
    }

    *m_volume=volume;

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

    const size_t VERSION2_MAGIC_LEN=9;
    char VERSION2_MAGIC[]="version2";
    char version2magic[VERSION2_MAGIC_LEN];

    istr.read(version2magic, VERSION2_MAGIC_LEN);
    if( !istr.good()){
        setError("Reading file id2 failed!");
        return false;
    }
    m_version=1;

    if(strncmp(version2magic, VERSION2_MAGIC, VERSION2_MAGIC_LEN)==0){
        m_version=2;
    }
    else{
        istr.seekg(-VERSION2_MAGIC_LEN, std::ios::cur);
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


bool VolumeReader::readDomainAndType(std::ifstream & istr, Domain &domain, VolumeType &type){

    qint16 dom;
    istr.read((char*)&dom, sizeof(dom));
    domain=static_cast<Domain>(dom);

    qint16 typ;
    istr.read((char*)&typ, sizeof(typ));
    type=static_cast<VolumeType>(typ);

    if(!istr.good()){
        setError("Reading domain and type failed!");
        return false;
    }

    return true;
}

/*
bool VolumeReader::readData(std::ifstream& istr, float* p, size_t count ){

    istr.read( (char*)p, count*sizeof(float));
    if(!istr.good()){
        setError("Reading volume data failed!");
        return false;
    }

    return true;
}
*/

bool VolumeReader::readData(std::ifstream& istr, float* p, size_t count ){

    const size_t CHUNK_SIZE=1024*1024;

    size_t n_read=0;

    while(n_read<count){

        size_t floats_to_read=std::min( CHUNK_SIZE, count-n_read);

        istr.read( (char*)(p + n_read), floats_to_read*sizeof(float));
        if(!istr.good()) break;

        n_read+=floats_to_read;

        int perc=100*n_read/count;
        emit percentDone(perc);
        qApp->processEvents();
    }

    if( !istr.good()){
        setError("Reading volume data failed!");
        return false;
    }

    return true;
}

void VolumeReader::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}
