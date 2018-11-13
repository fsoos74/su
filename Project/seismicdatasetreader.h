#ifndef SEISMICDATASETREADER_H
#define SEISMICDATASETREADER_H


#include "seismicdatasetinfo.h"
#include<QtSql/QtSql>
#include <memory>
#include <stdexcept>
#include <segyreader.h>
#include <gather.h>
#include <qobject.h>
#include"projectgeometry.h"

class SeismicDatasetReader : public QObject
{
    Q_OBJECT
public:

    class Exception : public std::runtime_error{
    public:
        Exception(const QString& msg) : std::runtime_error(msg.toStdString().c_str()){
        }
    };

    SeismicDatasetReader(const SeismicDatasetInfo&, QObject* parent=nullptr);

    void close();

    std::shared_ptr<seismic::SEGYReader> segyReader()const{
        return m_reader;
    }

    const SeismicDatasetInfo& info()const{
        return m_info;
    }


    // sequential access
    bool good();   // can read trace
    ssize_t sizeTraces();
    ssize_t tellTrace();
    void seekTrace(ssize_t no);
    seismic::Trace readTrace();
    std::shared_ptr<seismic::Gather> readGather(QString key, ssize_t maxTraces=10000);

    // sorted access
    std::shared_ptr<seismic::Trace> readFirstTrace( const QString& key1, const QString& value1);
    std::shared_ptr<seismic::Trace> readFirstTrace( const QString& key1, const QString& value1,
                                                    const QString& key2, const QString& value2);
    std::shared_ptr<seismic::Gather> readGather( const QString& key1, const QString& min1, const QString& max1,
                                                 size_t maxTraces=999999999);
    std::shared_ptr<seismic::Gather> readGather( const QString& key1, const QString& min1, const QString& max1,
                                                 const QString& key2, const QString& min2, const QString& max2,
                                                 size_t maxTraces=999999999);
    std::shared_ptr<seismic::Gather> readGather( const QString& key1, const QString& key2,
                                                 QVector<std::pair<QString,QString>> values,
                                                 size_t maxTraces=999999999);
    int minInline();
    int maxInline();
    int minCrossline();
    int maxCrossline();

    std::pair<int, int> firstTraceInlineCrossline();
    bool extractGeometry(ProjectGeometry&);

    void setOrder(const QString& key1, bool ascending1, const QString& key2, bool ascending2, const QString& key3, bool ascending3 );

signals:
    void started(int);
    void progress(int);
    void finished();

private:

    void addDData(seismic::Trace& trc);

    void openDatabase();
    void openSEGYFile();
    void closeDatabase();
    void closeSEGYFile();

    SeismicDatasetInfo                      m_info;
    QSqlDatabase                            m_db;
    std::shared_ptr<seismic::SEGYReader>    m_reader;
    QString                                 m_order;
};

#endif // SEISMICDATASETREADER_H
