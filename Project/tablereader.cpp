#include "tablereader.h"

#include<cstring>
#include<iostream>
#include<QString>


TableReader::TableReader(Table* t ):m_table(t){

}

bool TableReader::readFromFile(const QString& path){

    QFile file(path);
    file.open(QFile::ReadOnly);
    if(file.error()){
        setError(file.errorString());
        return false;
    }

    if( !readMagic(file)){
        return false;
    }

    QString key1;
    QString key2;
    qint64 size;
    qint8 mu;

    if(!readKeysAndSize(file, key1, key2, size, mu)){
        return false;
    }

    m_key1=key1;
    m_key2=key2;
    m_size=size;
    m_multi=static_cast<bool>(mu);

    if( !m_table ) return true;

    Table table(m_key1, m_key2, m_multi);

    if(!readData(file, table)){
        return false;
    }

    *m_table=table;

    return true;
}


bool TableReader::readMagic(QFile& file){

    const size_t MAGIC_LEN=18;
    char MAGIC[]="Binary Table File";
    char magic[MAGIC_LEN];

    file.read((char*)magic, MAGIC_LEN);
    if( file.error() ){
        setError(QString("Reading file id failed: ")+file.errorString());
        return false;
    }

    if(strncmp(magic, MAGIC, MAGIC_LEN!=0)){
        setError("Wrong file id!");
        return false;
    }

    return true;
}


bool TableReader::readKeysAndSize(QFile& file, QString &key1, QString &key2, qint64 &size, qint8 &multi){

    char cbuf[256];

    qint8 s1;
    file.read((char*) &s1, sizeof(s1));
    file.read(&cbuf[0], s1);
    cbuf[s1]='\0';
    key1=QString(cbuf);

    qint8 s2;
    file.read((char*) &s2, sizeof(s2));
    file.read(&cbuf[0], s2);
    cbuf[s2]='\0';
    key2=QString(cbuf);

    qint64 s;
    file.read((char*)&s, sizeof(s));
    size=s;

    qint8 m;
    file.read((char*)&m,sizeof(m));
    multi=static_cast<bool>(m);

    if(file.error()){
        setError(QString("Reading keys and size failed: ")+file.errorString());
        return false;
    }

    return true;
}

bool TableReader::readData(QFile& file, Table& table ){

    int n_read=0;

    //std::cout<<"size="<<m_size<<std::endl;
    qint32 k1,k2;
    while(n_read<m_size){


        file.read((char*)&k1, sizeof(k1));
        file.read((char*)&k2, sizeof(k2));
        double val;
        file.read((char*)&val, sizeof(val));

        table.insert(k1,k2,val);
        n_read++;
        //std::cout<<"n="<<n_read<<" k1="<<k1<<" k2="<<k2<<" val="<<val<<std::endl<<std::flush;
    }

    if( file.error() || n_read!=m_size){
        setError("Reading table data failed!");
        return false;
    }

    return true;
}

void TableReader::setError(const QString& msg){
    m_error=true;
    m_errorString=msg;
}
