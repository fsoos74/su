#ifndef VOLUMEWRITER2_H
#define VOLUMEWRITER2_H


#include<QString>
#include<QFile>
#include<QObject>

#include<volume.h>

class VolumeWriter2 : public QObject
{
Q_OBJECT
public:

    VolumeWriter2(const QString& name, QObject* parent=nullptr );

    bool open(const Grid3DBounds& bounds, const Domain& domain, const VolumeType& type);
    void flush();
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

    bool write(const Volume& volume);   // whole in inlines
    bool writeK(const Volume& volume);  // whole timeslices, horizontal geom must be the same, volume ft is used to find start sample
    void removeFile();

private:

   bool writeMagic(QFile&);
   bool writeBounds(QFile&, const Grid3DBounds&);
   bool writeDomainAndType(QFile&, const Domain&, const VolumeType&);
   bool writeData(const char* p, int il0, int nil);
   bool writeDataK(const char* p, int k0, int nk);

   void setError(const QString& msg);

   QFile m_file;
   Grid3DBounds m_bounds;
   qint64 m_dataStart=0;
   QString m_errorString;
};

#endif // VOLUMEWRITER2_H
