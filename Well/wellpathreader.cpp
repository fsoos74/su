#include "wellpathreader.h"

WellPathReader::WellPathReader(WellPath& p):m_path(p){

}

bool WellPathReader::readFromStream(std::ifstream& istr){

    if( !readMagic(istr)){
        return false;
    }

    qint64 size;
    if(!readHeader(istr, size )){
        return false;
    }

    QVector<QVector3D> data(size);

    if(!readData(istr, data.data(), data.size())){
        return false;
    }

    m_path=WellPath(data);

    return true;
}


bool WellPathReader::readMagic(std::ifstream& istr){

    const size_t MAGIC_LEN=21;
    char MAGIC[]="Binary Well Path File";

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


bool WellPathReader::readHeader(std::ifstream& istr, qint64& size){

    istr.read((char*)&size, sizeof(size));

    if(!istr.good()){
        setError("Reading header failed!");
        return false;
    }

    return true;
}

bool WellPathReader::readData(std::ifstream& istr, QVector3D* p, qint64 count ){

    istr.read( (char*)p, count*sizeof(QVector3D));
    if(!istr.good()){
        setError("Reading log data failed!");
        return false;
    }

    return true;
}


void WellPathReader::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}
