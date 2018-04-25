#include "seismicdatasetwriter.h"

#include <xsiwriter.h>

SeismicDatasetWriter::SeismicDatasetWriter(const SeismicDatasetInfo& info):m_info(info)
{
    openDatabase();
    openSEGYFile();
}


void SeismicDatasetWriter::writeTrace(const seismic::Trace& trc){

    m_writer->write_trace(trc);

    const seismic::Header& hdr=trc.header();
    m_addTraceQuery.addBindValue(static_cast<int>(m_size));
    int cdp=int(hdr.at("cdp").intValue());
    m_addTraceQuery.addBindValue(cdp);
    m_addTraceQuery.addBindValue( (info().dimensions()==3) ? int(hdr.at("iline").intValue()) : 1 );
    m_addTraceQuery.addBindValue( (info().dimensions()==3) ? int(hdr.at("xline").intValue()) : cdp );
    m_addTraceQuery.addBindValue(hdr.at("cdpx").floatValue());
    m_addTraceQuery.addBindValue(hdr.at("cdpy").floatValue());
    m_addTraceQuery.addBindValue( (info().mode()==SeismicDatasetInfo::Mode::Prestack) ? hdr.at("offset").floatValue() : 0);

    if( !m_addTraceQuery.exec() ) throw Exception("Failed to add trace index!");

    m_size++;
}

void SeismicDatasetWriter::writeGather(std::shared_ptr<seismic::Gather> gather){

}

void SeismicDatasetWriter::close(){

   // m_writer->close();

    m_db.commit();

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

    m_addTraceQuery=QSqlQuery(m_db);
    m_addTraceQuery.prepare(QString("INSERT INTO %1 (trace, cdp, iline, xline, x, y, offset) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?)").arg(tableName));

    m_db.transaction();
}


void SeismicDatasetWriter::openSEGYFile(){

    seismic::SEGYInfo segyInfo;
    //segyInfo.set
    segyInfo.setScalco(1./10);          // XXX
    segyInfo.setSwap(false);    // native format
    segyInfo.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    //seismic::SEGYWriter writer( fileName.toStdString(), info, textHeader, bhdr );

    seismic::XSIWriter segyInfoWriter(segyInfo);

    QFile segyInfoFile(m_info.infoPath());
    segyInfoFile.open(QFile::WriteOnly | QFile::Text);
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
    textHeaderStr.push_back("C01 SEGY created with AVO-Detect");
    textHeaderStr.push_back(QString("C02 Created: %1").arg(QDateTime::currentDateTime().toString()).toStdString());
    textHeaderStr.push_back("C03 Format: IEEE");
    textHeaderStr.push_back("C04");
    textHeaderStr.push_back(QString("C05 Dataset: %1").arg(m_info.name()).toStdString());
    textHeaderStr.push_back(QString("C06 Domain=%1").arg(datasetDomainToString(m_info.domain())).toStdString());
    textHeaderStr.push_back(QString("C07 Mode=%1").arg(datasetModeToString(m_info.mode())).toStdString() );
    textHeaderStr.push_back(QString("C08 Trace Start=%1").arg(1000*m_info.ft()).toStdString());
    textHeaderStr.push_back(QString("C09 Trace Sampling Interval=%1").arg(1000*m_info.dt()).toStdString());
    textHeaderStr.push_back(QString("C10 Trace Number of Samples=%1").arg(m_info.nt()).toStdString());
    textHeaderStr.push_back("C11");
    textHeaderStr.push_back("C12 Trace Header Definition:");
    textHeaderStr.push_back("C13 Time of first sample [ms]  bytes 109-110");
    textHeaderStr.push_back("C14 Inline                     bytes 189-192");
    textHeaderStr.push_back("C15 Crossline                  bytes 193-196");
    textHeaderStr.push_back("C16 X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("C17 Y-Coordinate               bytes 185-188");

    for( auto i=18; i<=40; i++){
        textHeaderStr.push_back(QString("C%1").arg(QString::number(i)).toStdString());
    }

    return seismic::convertToRaw(textHeaderStr);
}

seismic::Header SeismicDatasetWriter::buildBinaryHeader()const{

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(m_info.nt() );
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(1000000*m_info.dt());

    return bhdr;
}

