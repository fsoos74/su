#ifndef LOGREADER_H
#define LOGREADER_H

#include<memory>
#include<fstream>
#include<memory>
#include<string>
#include<QString>
#include<log.h>


class LogReader
{
public:

    LogReader(Log* v = nullptr );

    qreal z0()const{
        return m_z0;
    }

    qreal dz()const{
        return m_dz;
    }

    qint64 nz()const{
        return m_nz;
    }

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool readFromStream(std::ifstream&);

private:

    bool readMagic(std::ifstream&);
    bool readHeader(std::ifstream&, std::string&, std::string&, std::string&, qreal&, qreal&, qint64&);
    bool readData(std::ifstream&, double*, size_t);
    void setError(const QString& msg);

    std::string m_name;
    std::string m_unit;
    std::string m_descr;
    qreal m_z0=0;
    qreal m_dz=0;
    qint64 m_nz=0;
    Log* m_log;
    bool m_error=false;
    QString m_errorString;
};

#endif // LOGREADER_H
