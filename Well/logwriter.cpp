#include "logwriter.h"

#include"stringio.h"

LogWriter::LogWriter(const Log& l):m_log(l),m_error(false){

}


bool LogWriter::writeToStream(std::ofstream& os){

    if(!writeMagic(os)){
        setError("Writing file id failed!");
        return false;
    }

    if(!writeHeader(os)){
        setError("Writing header failed!");
        return false;
    }

    if(!writeData(os)){
        setError("Writing data failed!");
        return false;
    }

    return true;
}

bool LogWriter::writeMagic(std::ofstream& os){

    const size_t MAGIC_LEN=16;
    char MAGIC[]="Binary Log File";

    os.write(MAGIC, MAGIC_LEN);

    return os.good();
}

bool LogWriter::writeHeader(std::ofstream& os){

    writeString(os, m_log.name() );
    writeString(os, m_log.unit() );
    writeString(os, m_log.descr() );
    qreal z0=m_log.z0();
    os.write((char*)&z0, sizeof(z0));
    qreal dz=m_log.dz();
    os.write((char*)&dz, sizeof(dz));
    qint64 nz=m_log.nz();
    os.write((char*)&nz,sizeof(nz));

    return os.good();
}

bool LogWriter::writeData(std::ofstream& os){

    auto l = const_cast<Log&>(m_log);   // m_log will NOT be modified!

    os.write( (char*)(&(l[0])), m_log.nz()*sizeof(double) );
    return os.good();

}

void LogWriter::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}

