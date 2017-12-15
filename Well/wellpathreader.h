#ifndef WELLPATHREADER_H
#define WELLPATHREADER_H

#include<memory>
#include<fstream>
#include<memory>
#include<string>
#include<QString>
#include<wellpath.h>


class WellPathReader
{
public:

    WellPathReader(WellPath&);

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool readFromStream(std::ifstream&);

private:

    bool readMagic(std::ifstream&);
    bool readHeader(std::ifstream&, qint64&);
    bool readData(std::ifstream&, QVector3D*, qint64 count);
    void setError(const QString& msg);

    WellPath& m_path;
    bool m_error=false;
    QString m_errorString;
};

#endif // WELLPATHREADER_H
