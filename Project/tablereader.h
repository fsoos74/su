#ifndef TABLEREADER_H
#define TABLEREADER_H

#include<memory>

#include<fstream>
#include<memory>
#include<table.h>
#include<QString>
#include<QFile>

class TableReader
{

public:

    TableReader(Table* v = nullptr);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool readFromFile(const QString& path);

    int size()const{
        return m_size;
    }

    bool isMulti()const{
        return m_multi;
    }

    QString key1()const{
        return m_key1;
    }

    QString key2()const{
        return m_key2;
    }

private:

    bool readMagic(QFile&);
    bool readKeysAndSize(QFile&, QString& key1, QString& key2, qint64& size, qint8& multi);
    bool readData(QFile&, Table&);
    void setError(const QString& msg);

    QString m_key1;
    QString m_key2;
    int m_size;
    bool m_multi;
    Table* m_table;
    bool m_error=false;
    QString m_errorString;
};

#endif // TABLEREADER_H
