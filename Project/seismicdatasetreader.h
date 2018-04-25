#ifndef SEISMICDATASETREADER_H
#define SEISMICDATASETREADER_H


#include "seismicdatasetinfo.h"
#include<QtSql/QtSql>
#include <memory>
#include <stdexcept>
#include <segyreader.h>
#include <gather.h>

#include"projectgeometry.h"

class SeismicDatasetReader
{
public:

    class Exception : public std::runtime_error{
    public:
        Exception(const QString& msg) : std::runtime_error(msg.toStdString().c_str()){
        }
    };

    SeismicDatasetReader(const SeismicDatasetInfo&);

    void close();

    std::shared_ptr<seismic::SEGYReader> segyReader()const{
        return m_reader;
    }

    const SeismicDatasetInfo& info()const{
        return m_info;
    }

    std::shared_ptr<seismic::Trace> readFirstTrace( const QString& key1, const QString& value1, const QString& key2, const QString& value2);

    std::shared_ptr<seismic::Gather> readGather( const QString& key1, const QString& min1, const QString& max1,
                                                 size_t maxTraces=999999999);
    std::shared_ptr<seismic::Gather> readGather( const QString& key1, const QString& min1, const QString& max1,
                                                 const QString& key2, const QString& min2, const QString& max2,
                                                 size_t maxTraces=999999999);
    int minInline();
    int maxInline();
    int minCrossline();
    int maxCrossline();

    std::pair<int, int> firstTraceInlineCrossline();
    bool extractGeometry(ProjectGeometry&);

    void setOrder(const QString& key1, bool ascending1, const QString& key2, bool ascending2, const QString& key3, bool ascending3 );

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
