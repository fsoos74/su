#ifndef SEISMICDATASETWRITER_H
#define SEISMICDATASETWRITER_H

#include "seismicdatasetinfo.h"
#include<QtSql/QtSql>
#include <memory>
#include <stdexcept>
#include <segywriter.h>
#include <gather.h>


class SeismicDatasetWriter
{
public:

    class Exception : public std::runtime_error{
    public:
        Exception(const QString& msg) : std::runtime_error(msg.toStdString().c_str()){
        }
    };

    SeismicDatasetWriter(const SeismicDatasetInfo&, unsigned dt_us, unsigned ns);

    std::shared_ptr<seismic::SEGYWriter> segyWriter()const{
        return m_writer;
    }

    const SeismicDatasetInfo& info()const{
        return m_info;
    }

    void writeTrace(std::shared_ptr<seismic::Trace>);

    void writeGather(std::shared_ptr<seismic::Gather>);

    void close();

private:

    void openDatabase();
    void openSEGYFile();
    seismic::SEGYTextHeader buildTextualHeader()const;
    seismic::Header buildBinaryHeader()const;


    SeismicDatasetInfo                      m_info;
    QSqlDatabase                            m_db;
    QSqlQuery m_addTraceQuery;
    std::shared_ptr<seismic::SEGYWriter>    m_writer;
    unsigned m_dt_us;
    unsigned m_ns;
    size_t m_size=0;
};

#endif // SEISMICDATASETWRITER_H
