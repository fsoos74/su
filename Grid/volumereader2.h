#ifndef VOLUMEREADER2_H
#define VOLUMEREADER2_H


#include<memory>
#include<volume.h>
#include<QString>
#include<QFile>
#include <QObject>

class VolumeReader2 : public QObject
{
    Q_OBJECT

public:

    VolumeReader2(const QString& fname, QObject* parent=nullptr );

    bool open();
    void close();
    bool good(){
        return (m_file.error()==QFile::FileError::NoError);
    }

    QString lastError()const{
        QString str=m_errorString;
        if( !m_file.errorString().isEmpty()){
            str+=": "+m_file.errorString();
        }
        return str;
    }

    const Grid3DBounds& bounds()const{
        return m_bounds;
    }

    Domain domain()const{
        return m_domain;
    }

    VolumeType type()const{
        return m_type;
    }

    // read the full volume, returns null pointer on error
    std::shared_ptr<Volume> read();

    // read inlines [il1..il2], return them as volume, null ptr on error
    std::shared_ptr<Volume> readIl(int il1, int il2);

    // read crosslines [xl1..xl2], return them as volume, null ptr on error
    std::shared_ptr<Volume> readXl(int xl1, int ix2);

    // read time/depth slices at samples [k1..k2], return them as volume, null ptr on error
    std::shared_ptr<Volume> readK(int k1, int k2);

signals:
    void percentDone(int);

private:

    bool readMagic(QFile&);
    bool readBounds(QFile&, Grid3DBounds&);
    bool readDomainAndType(QFile&, Domain&, VolumeType&);
    std::shared_ptr<Volume>  readData(const Grid3DBounds& bounds);
    bool readDataIl(char* p, int il0, int nil);
    bool readDataXl(char* p, int xl0, int nxl);
    bool readDataK(char* p, int k0, int nk);
    void setError(const QString& msg);

    QFile m_file;
    int m_version=0;
    Grid3DBounds m_bounds;
    Domain m_domain=Domain::Other;
    VolumeType m_type=VolumeType::Other;
    qint64 m_dataStart;
    QString m_errorString;
    //bool m_error=false;
};

#endif // VOLUMEREADER_H
