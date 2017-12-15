#ifndef VOLUMEREADER_H
#define VOLUMEREADER_H

#include<memory>

#include<fstream>
#include<memory>
#include<volume.h>
#include<QString>

#include <QObject>

class VolumeReader : public QObject
{
    Q_OBJECT

public:

    VolumeReader(Volume* v = nullptr, QObject* parent=nullptr );

    bool hasError()const{
        return m_error;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    bool readFromStream(std::ifstream&);

    const Grid3DBounds& bounds()const{
        return m_bounds;
    }

    Domain domain()const{
        return m_domain;
    }

    VolumeType type()const{
        return m_type;
    }

signals:
    void percentDone(int);

private:

    bool readMagic(std::ifstream&);
    bool readBounds(std::ifstream&, Grid3DBounds&);
    bool readDomainAndType(std::ifstream&, Domain&, VolumeType&);
    bool readData(std::ifstream&, float*, size_t);
    void setError(const QString& msg);

    Grid3DBounds m_bounds;
    Domain m_domain=Domain::Other;
    VolumeType m_type=VolumeType::Other;
    Volume* m_volume;
    bool m_error=false;
    QString m_errorString;
    int m_version=0;
};

#endif // VOLUMEREADER_H
