#include "logreader.h"

#include<cstring>

#include<stringio.h>

LogReader::LogReader(Log* v):m_log(v){

}

bool LogReader::readFromStream(std::ifstream& istr){

    if( !readMagic(istr)){
        return false;
    }

    if(!readHeader(istr, m_name, m_unit, m_descr, m_z0, m_dz, m_nz )){
        return false;
    }

    if( !m_log ) return true;   // read only header

    Log l(m_name, m_unit, m_descr, m_z0, m_dz, m_nz);

    if(!readData(istr, &l[0], l.nz())){
        return false;
    }

    *m_log=l;

    return true;
}


bool LogReader::readMagic(std::ifstream& istr){

    const size_t MAGIC_LEN=16;
    char MAGIC[]="Binary Log File";
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


bool LogReader::readHeader(std::ifstream& istr, std::string& name, std::string& unit, std::string& descr, qreal& z0, qreal& dz, qint64& nz){

    name=readString(istr);
    unit=readString(istr);
    descr=readString(istr);
    istr.read((char*)&z0, sizeof(z0));
    istr.read((char*)&dz, sizeof(dz));
    istr.read((char*)&nz, sizeof(nz));

    if(!istr.good()){
        setError("Reading header failed!");
        return false;
    }

    return true;
}

bool LogReader::readData(std::ifstream& istr, double* p, size_t count ){

    istr.read( (char*)p, count*sizeof(double));
    if(!istr.good()){
        setError("Reading log data failed!");
        return false;
    }

    return true;
}


void LogReader::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}
