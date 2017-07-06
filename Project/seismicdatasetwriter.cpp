#include "seismicdatasetwriter.h"

#include <xsiwriter.h>

SeismicDatasetWriter::SeismicDatasetWriter(const SeismicDatasetInfo& info, unsigned dt_us, unsigned ns):m_info(info), m_dt_us(dt_us), m_ns(ns)
{
    openDatabase();
    openSEGYFile();
}


void SeismicDatasetWriter::writeTrace(std::shared_ptr<seismic::Trace> trc){

    m_writer->write_trace(*trc);

    const seismic::Header& hdr=trc->header();
    m_addTraceQuery.addBindValue(static_cast<int>(m_size));
    m_addTraceQuery.addBindValue(int(hdr.at("cdp").intValue()));
    m_addTraceQuery.addBindValue(int(hdr.at("iline").intValue()));
    m_addTraceQuery.addBindValue(int(hdr.at("xline").intValue()));
    m_addTraceQuery.addBindValue(hdr.at("cdpx").floatValue());
    m_addTraceQuery.addBindValue(hdr.at("cdpy").floatValue());
    m_addTraceQuery.addBindValue(hdr.at("offset").floatValue());

    if( !m_addTraceQuery.exec()) throw Exception("Failed to add trace index!");

    m_size++;
}

void SeismicDatasetWriter::writeGather(std::shared_ptr<seismic::Gather> gather){

}

void SeismicDatasetWriter::close(){

   // m_writer->close();
    m_db.close();
}

void SeismicDatasetWriter::openDatabase(){

    QString tableName="map";
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_info.indexPath());
    m_db.open();

    QSqlQuery queryCreateTable( QString("CREATE TABLE IF NOT EXISTS %1 ("
            "trace integer primary key, cdp integer,  iline  integer, xline  integer, x  real,  y real, offset real"
            ")").arg(tableName) );
    if( !queryCreateTable.exec()) throw Exception(QString("Adding dataset index \"%1\" failed!").arg(m_info.indexPath()));

    m_addTraceQuery.prepare(QString("INSERT INTO %1 (trace, cdp, iline, xline, x, y, offset) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?)").arg(tableName));
}


void SeismicDatasetWriter::openSEGYFile(){

    seismic::SEGYInfo segyInfo;
    seismic::XSIWriter segyInfoWriter(segyInfo);
    QFile segyInfoFile(m_info.infoPath());
    if( !segyInfoWriter.writeFile(&segyInfoFile)){
        throw Exception( QString("Wriing segy info file \"%1\" failed!").arg(m_info.infoPath()));
    }

    auto th=buildTextualHeader();
    auto bh=buildBinaryHeader();
    m_writer = std::shared_ptr<seismic::SEGYWriter>(new seismic::SEGYWriter(m_info.path().toStdString(), segyInfo, th, bh) );

    if( !m_writer ){
        throw Exception("Open segy writer failed!");
    }

    m_writer->write_leading_headers();
}

seismic::SEGYTextHeader SeismicDatasetWriter::buildTextualHeader()const{

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("SEGY created with AVO-Detect");
    textHeaderStr.push_back(QString("Dataset: %1").arg(m_info.name()).toStdString());
    textHeaderStr.push_back("");
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Time of first sample [ms]  bytes 109-110");
    textHeaderStr.push_back("Inline                     bytes 189-192");
    textHeaderStr.push_back("Crossline                  bytes 193-196");
    textHeaderStr.push_back("X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("Y-Coordinate               bytes 185-188");

    return seismic::convertToRaw(textHeaderStr);
}

seismic::Header SeismicDatasetWriter::buildBinaryHeader()const{

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(m_ns);
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(m_dt_us);

    return bhdr;
}

