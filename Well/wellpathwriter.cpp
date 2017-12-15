#include "wellpathwriter.h"

#include<stringio.h>

WellPathWriter::WellPathWriter(const WellPath& p):m_path(p),m_error(false){

}


bool WellPathWriter::writeToStream(std::ofstream& os){

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

bool WellPathWriter::writeMagic(std::ofstream& os){

    const size_t MAGIC_LEN=21;
    char MAGIC[]="Binary Well Path File";

    os.write(MAGIC, MAGIC_LEN);

    return os.good();
}


bool WellPathWriter::writeHeader(std::ofstream& os){

    qint64 s=m_path.size();

    os.write( (char*)&s, sizeof(s) );

    return os.good();
}

bool WellPathWriter::writeData(std::ofstream& os){

    os.write( (char*) m_path.data(), m_path.size()*sizeof(QVector3D) );
    return os.good();
}

void WellPathWriter::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}

