#include "tablewriter.h"

TableWriter::TableWriter(const Table& t):m_table(t),m_error(false){

}


bool TableWriter::writeToFile(const QString& path){

    QFile file(path);
    file.open(QFile::WriteOnly|QFile::Truncate);
    if(file.error()){
        setError(file.errorString());
        return false;
    }

    if(!writeMagic(file)){
        setError(QString("Writing file id failed: ")+file.errorString());
        return false;
    }

    if(!writeKeysAndSize(file)){
        setError(QString("Writing keys and size failed: ")+file.errorString());
        return false;
    }


    if(!writeData(file)){
        setError(QString("Writing data failed: ")+file.errorString());
        return false;
    }

    file.flush();
    file.close();

    return true;
}

bool TableWriter::writeMagic(QFile& file){

    const size_t MAGIC_LEN=18;
    char MAGIC[]="Binary Table File";

    file.write(MAGIC, MAGIC_LEN);

    return !file.error();
}

bool TableWriter::writeKeysAndSize(QFile& file){

    auto k1=m_table.key1().toStdString();
    qint8 s1=k1.size();
    file.write((char*) &s1, sizeof(s1));
    file.write(k1.c_str(), s1);

    auto k2=m_table.key2().toStdString();
    qint8 s2=k2.size();
    file.write((char*) &s2, sizeof(s2));
    file.write(k2.c_str(), s2);

    qint64 s=m_table.size();
    file.write((char*)&s, sizeof(s));

    qint8 m=static_cast<qint8>(m_table.isMulti());
    file.write((char*)&m,sizeof(m));

    return !file.error();
}

bool TableWriter::writeData(QFile& file){

    int n=0;
    for( auto it=m_table.cbegin(); it!=m_table.cend(); it++){
        auto key=m_table.split_key( it.key() );
        qint32 k1=key.first;
        qint32 k2=key.second;
        auto val=it.value();
        file.write((char*)&k1, sizeof(k1));
        file.write((char*)&k2, sizeof(k2));
        file.write((char*)&val, sizeof(val));
        //std::cout<<"n="<<n<<" k1="<<k1<<" k2="<<k2<<" val="<<val<<std::endl;
        n++;
    }

    return !file.error();
}

void TableWriter::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}

