#include "seismicdatasetreader.h"
#include<xsireader.h>

#include<QProgressDialog>

SeismicDatasetReader::SeismicDatasetReader(const SeismicDatasetInfo& info, QObject* parent):QObject(parent),m_info(info)
{
    openDatabase();
    openSEGYFile();
}


void SeismicDatasetReader::close(){
    closeDatabase();
    closeSEGYFile();
}

ssize_t SeismicDatasetReader::sizeTraces(){
    return m_reader->trace_count();
}

bool SeismicDatasetReader::good(){
    if( !m_reader) return false;
    return m_reader->current_trace() < m_reader->trace_count();
}

ssize_t SeismicDatasetReader::tellTrace(){
    return m_reader->current_trace();
}

void SeismicDatasetReader::seekTrace(ssize_t no){

    m_reader->seek_trace(no);
}

seismic::Trace SeismicDatasetReader::readTrace(){
    auto trc=m_reader->read_trace();
    addDData(trc);
    return trc;
}

std::shared_ptr<seismic::Gather> SeismicDatasetReader::readGather(QString key, ssize_t maxTraces){
    auto gt = m_reader->read_gather(key.toStdString().c_str(), maxTraces);
    if(!gt) return gt;
    for( auto it=gt->begin(); it!=gt->end(); it++){
        addDData(*it);
    }

    return gt;
}

std::pair<int, int> SeismicDatasetReader::firstTraceInlineCrossline(){

    QSqlQuery query("first inline and crossline", m_db);
    query.prepare(QString("select * from %1 where trace=0").arg("map") );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.next();
    return std::pair<int, int>(query.value(2).toInt(), query.value(3).toInt() );

}

int SeismicDatasetReader::minInline(){
    QSqlQuery query("min inline", m_db);
    query.prepare(QString("select min(iline) from %1").arg("map") );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.next();
    return query.value(0).toInt();

}

int SeismicDatasetReader::maxInline(){
    QSqlQuery query( "max inline", m_db );
    query.prepare(QString("select max(iline) from %1").arg("map") );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.next();
    return query.value(0).toInt();

}

int SeismicDatasetReader::minCrossline(){
    QSqlQuery query("min crossline", m_db);
    query.prepare(QString("select min(xline) from %1 order by xline").arg("map") );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.next();
    return query.value(0).toInt();

}

int SeismicDatasetReader::maxCrossline(){
    QSqlQuery query("max crossline", m_db);
    query.prepare(QString("select max(xline) from %1").arg("map") );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.next();
    return query.value(0).toInt();

}

// new version has P1(min il, min xl), P2(min il, max xl), P3(max il, min xl)
// if pairs from traces don't exist they will be computed
bool SeismicDatasetReader::extractGeometry(ProjectGeometry& geom){

    int il1=minInline();
    int il2=minInline();
    int il3=maxInline();
    int xl1=-1;
    int xl2=-1;
    int xl3=-1;

    // min xline for min iline (point1)
    QSqlQuery queryxl1("xl point1", m_db);
    queryxl1.prepare(QString("select xline from map where iline==%1 order by 1 asc").arg(QString::number(il1) ) );
    if( !queryxl1.exec()){
        return false;
    }
    if( !queryxl1.next()) return false;
    xl1=queryxl1.value("xline").toInt();

    std::cout<<"temp P1 il="<<il1<<" xl="<<xl1<<std::endl;

    // point 1
    QSqlQuery query1("point1", m_db);
    query1.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il1), QString::number(xl1) ) );
    if( !query1.exec()){
        return false;
    }
    if( !query1.next()) return false;

    qreal x1=query1.value("x").toDouble();
    qreal y1=query1.value("y").toDouble();

    std::cout<<"temp P1 x="<<x1<<" y="<<y1<<std::endl;


    // max xline for min iline (point2)
    QSqlQuery queryxl2("xl point2", m_db);
    queryxl2.prepare(QString("select xline from map where iline==%1 order by 1 desc").arg(QString::number(il2) ) );
    if( !queryxl2.exec()){
        return false;
    }
    if( !queryxl2.next()) return false;
    xl2=queryxl2.value("xline").toInt();

    std::cout<<"temp P2 il="<<il2<<" xl="<<xl2<<std::endl;

    // point 2
    QSqlQuery query2("point2", m_db);
    query2.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il2), QString::number(xl2) ) );
    if( !query2.exec()){
        return false;
    }
    if( !query2.next()) return false;

    qreal x2=query2.value("x").toDouble();
    qreal y2=query2.value("y").toDouble();

    std::cout<<"P2 x="<<x2<<" y="<<y2<<std::endl;


    // min xline for max iline (point3)
    QSqlQuery queryxl3("xl point3", m_db);
    queryxl3.prepare(QString("select xline from map where iline==%1 order by 1 asc").arg(QString::number(il3) ) );
    if( !queryxl3.exec()){
        return false;
    }
    if( !queryxl3.next()) return false;
    xl3=queryxl3.value("xline").toInt();

    std::cout<<"temp P3 il="<<il3<<" xl="<<xl3<<std::endl;

    // point 3
    QSqlQuery query3("point3", m_db);
    query3.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il3), QString::number(xl3) ) );
    if( !query3.exec()){
        return false;
    }
    if( !query3.next()) return false;

    qreal x3=query3.value("x").toDouble();
    qreal y3=query3.value("y").toDouble();

    std::cout<<"temp P3 x="<<x3<<" y="<<y3<<std::endl;


    ProjectGeometry tmp;
    tmp.coords(0) = QPointF(x1,y1);
    tmp.coords(1) = QPointF(x2,y2);
    tmp.coords(2) = QPointF(x3,y3);

    tmp.lines(0) = QPoint(il1,xl1);
    tmp.lines(1) = QPoint(il2,xl2);
    tmp.lines(2) = QPoint(il3,xl3);

    if( !isValid(tmp)) return false;

    // recompute corner coordinates
    ProjectGeometry g;

    g.lines(0)=QPoint(minInline(), minCrossline());
    g.lines(1)=QPoint(minInline(), maxCrossline());
    g.lines(2)=QPoint(maxInline(), minCrossline());
    QTransform ilxl_to_xy, xy_to_ilxl;
    tmp.computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( int i=0; i<3; i++){
        g.coords(i)=ilxl_to_xy.map(g.lines(i));
    }

    if( !isValid(g)) return false;

    geom=g;

    return true;
}

/* latest working version from 11/30/16
 *
bool SeismicDatasetReader::extractGeometry(ProjectGeometry& geom){

    int il1=minInline();
    int il2=minInline();
    int il3=maxInline();
    int xl1=-1;
    int xl2=-1;
    int xl3=-1;

    // min xline for min iline (point1)
    QSqlQuery queryxl1("xl point1", m_db);
    queryxl1.prepare(QString("select xline from map where iline==%1 order by 1 asc").arg(QString::number(il1) ) );
    if( !queryxl1.exec()){
        return false;
    }
    if( !queryxl1.next()) return false;
    xl1=queryxl1.value("xline").toInt();

    std::cout<<"P1 il="<<il1<<" xl="<<xl1<<std::endl;

    // point 1
    QSqlQuery query1("point1", m_db);
    query1.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il1), QString::number(xl1) ) );
    if( !query1.exec()){
        return false;
    }
    if( !query1.next()) return false;

    qreal x1=query1.value("x").toDouble();
    qreal y1=query1.value("y").toDouble();

    std::cout<<"P1 x="<<x1<<" y="<<y1<<std::endl;


    // max xline for min iline (point2)
    QSqlQuery queryxl2("xl point2", m_db);
    queryxl2.prepare(QString("select xline from map where iline==%1 order by 1 desc").arg(QString::number(il2) ) );
    if( !queryxl2.exec()){
        return false;
    }
    if( !queryxl2.next()) return false;
    xl2=queryxl2.value("xline").toInt();

    std::cout<<"P2 il="<<il2<<" xl="<<xl2<<std::endl;

    // point 2
    QSqlQuery query2("point2", m_db);
    query2.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il2), QString::number(xl2) ) );
    if( !query2.exec()){
        return false;
    }
    if( !query2.next()) return false;

    qreal x2=query2.value("x").toDouble();
    qreal y2=query2.value("y").toDouble();

    std::cout<<"P2 x="<<x2<<" y="<<y2<<std::endl;


    // min xline for max iline (point3)
    QSqlQuery queryxl3("xl point3", m_db);
    queryxl3.prepare(QString("select xline from map where iline==%1 order by 1 asc").arg(QString::number(il3) ) );
    if( !queryxl3.exec()){
        return false;
    }
    if( !queryxl3.next()) return false;
    xl3=queryxl3.value("xline").toInt();

    std::cout<<"P3 il="<<il3<<" xl="<<xl3<<std::endl;

    // point 3
    QSqlQuery query3("point3", m_db);
    query3.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il3), QString::number(xl3) ) );
    if( !query3.exec()){
        return false;
    }
    if( !query3.next()) return false;

    qreal x3=query3.value("x").toDouble();
    qreal y3=query3.value("y").toDouble();

    std::cout<<"P3 x="<<x3<<" y="<<y3<<std::endl;


    ProjectGeometry g;
    g.coords(0) = QPointF(x1,y1);
    g.coords(1) = QPointF(x2,y2);
    g.coords(2) = QPointF(x3,y3);

    g.lines(0) = QPoint(il1,xl1);
    g.lines(1) = QPoint(il2,xl2);
    g.lines(2) = QPoint(il3,xl3);

    if( !isValid(g)) return false;

    geom=g;

    return true;
}
*/

/* last working version, problem: min xline not at min iline asf.
 bool SeismicDatasetReader::extractGeometry(ProjectGeometry& geom){

     int il1=minInline();
     int il2=il1;
     int il3=maxInline();
     int xl1=minCrossline();
     int xl2=maxCrossline();
     int xl3=xl1;

     // point 1
     QSqlQuery query1("point1", m_db);
     query1.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il1), QString::number(xl1) ) );
     if( !query1.exec()){
         return false;
     }
     if( !query1.next()) return false;

     qreal x1=query1.value("x").toDouble();
     qreal y1=query1.value("y").toDouble();


     // point 2
     QSqlQuery query2("point2", m_db);
     query2.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il2), QString::number(xl2) ) );
     if( !query2.exec()){
         return false;
     }
     if( !query2.next()) return false;

     qreal x2=query2.value("x").toDouble();
     qreal y2=query2.value("y").toDouble();


     // point 3
     QSqlQuery query3("point3", m_db);
     query3.prepare(QString("select * from map where iline==%1 and xline==%2 ").arg(QString::number(il3), QString::number(xl3) ) );
     if( !query3.exec()){
         return false;
     }
     if( !query3.next()) return false;

     qreal x3=query3.value("x").toDouble();
     qreal y3=query3.value("y").toDouble();



     ProjectGeometry g;
     g.setCoordinates(0, QPointF(x1,y1));
     g.setCoordinates(1, QPointF(x2,y2));
     g.setCoordinates(2, QPointF(x3,y3));

     g.setInlineAndCrossline(0, QPoint(il1,xl1));
     g.setInlineAndCrossline(1, QPoint(il2,xl2));
     g.setInlineAndCrossline(2, QPoint(il3,xl3));

     if( !isValid(g)) return false;

     geom=g;

     return true;
 }
*/

void SeismicDatasetReader::setOrder(const QString& key1, bool ascending1, const QString& key2, bool ascending2, const QString& key3, bool ascending3 ){

    QString order;

    if( !key1.isEmpty()){
        order.append(key1);
        if(ascending1){
            order.append(" ASC");
        }
        else{
            order.append(" DESC");
        }
    }

    if( !key2.isEmpty()){
        if( !order.isEmpty()){
            order.append(", ");
        }
        order.append(key2);
        if(ascending2){
            order.append(" ASC");
        }
        else{
            order.append(" DESC");
        }
    }

    if( !key3.isEmpty()){
        if( !order.isEmpty()){
            order.append(", ");
        }
        order.append(key3);
        if(ascending3){
            order.append(" ASC");
        }
        else{
            order.append(" DESC");
        }
    }

    if( !order.isEmpty()){
        m_order=QString("ORDER BY ")+order;
    }
    else{
        m_order=order;
    }
}

// adjust trace headers to dataset
void SeismicDatasetReader::addDData(seismic::Trace& trc ){

    if( m_info.dimensions()==2){
        trc.header()["iline"]=seismic::HeaderValue::makeIntValue(1);
        trc.header()["xline"]=trc.header()["cdp"]; //  seismic::HeaderValue::makeIntValue(xl);
    }

    if( m_info.mode()==SeismicDatasetInfo::Mode::Poststack){
        trc.header()["offset"]=seismic::HeaderValue::makeFloatValue(0);
    }
}


std::shared_ptr<seismic::Trace> SeismicDatasetReader::readFirstTrace( const QString& key1, const QString& value1){

    QSqlQuery query("firsttrace1", m_db);
    query.prepare(QString("select * from map where %1==%2 ").arg(key1, value1) );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    if( !query.next()) return std::shared_ptr<seismic::Trace>();

    bool ok=false;
    int traceNo=query.value("trace").toInt(&ok);
    if( !ok){
        throw Exception(QString("Accessing trace number failed!"));
    }

    m_reader->seek_trace(traceNo);

    auto trc = m_reader->read_trace();

    addDData(trc);

    return std::make_shared<seismic::Trace>(trc);
}



std::shared_ptr<seismic::Trace> SeismicDatasetReader::readFirstTrace( const QString& key1, const QString& value1,
                                                     const QString& key2, const QString& value2){

    QSqlQuery query("firsttrace2", m_db);
    QString str=QString("select * from map where %1==%2 and %3==%4 ").arg(key1, value1, key2, value2 );
    if( !query.exec(str)){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    if( !query.next()) return std::shared_ptr<seismic::Trace>();

    bool ok=false;
    int traceNo=query.value("trace").toInt(&ok);
    if( !ok){
        throw Exception(QString("Accessing trace number failed!"));
    }

    m_reader->seek_trace(traceNo);

    auto trc = m_reader->read_trace();

    addDData(trc);

    return std::make_shared<seismic::Trace>(trc);
}


std::shared_ptr<seismic::Gather> SeismicDatasetReader::readGather(
    const QString& key1, const QString& min1, const QString& max1, size_t maxTraces){

    QSqlQuery query("gather", m_db);
    query.prepare(QString("select * from map where %1>=%2 and %1<=%3 ").arg(key1,min1, max1 ) + m_order);
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }


    std::shared_ptr<seismic::Gather> gather(new seismic::Gather());

    while( query.next() && gather->size()<maxTraces){

        bool ok=false;
        int traceNo=query.value("trace").toInt(&ok);
        if( !ok){
            throw Exception(QString("Accessing trace number failed!"));
        }

        m_reader->seek_trace(traceNo);

        auto trc=m_reader->read_trace();

        addDData(trc);

        gather->push_back(trc);
    }

    return gather;
}

std::shared_ptr<seismic::Gather> SeismicDatasetReader::readGather( const QString& key1, const QString& min1, const QString& max1,
                                                                   const QString& key2, const QString& min2, const QString& max2,
                                                                   size_t maxTraces){

    QSqlQuery query("gather 2 keys", m_db);
    query.prepare(QString("select * from map where %1>=%2 and %1<=%3 and %4>=%5 and %4<=%6 ").arg(key1,min1, max1, key2, min2, max2 ) + m_order );
    if( !query.exec()){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    // determine size, size() does not work for sqlite
    query.last();
    int n=query.at();
    if(n>maxTraces) n=maxTraces;
    query.first();
    std::shared_ptr<seismic::Gather> gather(new seismic::Gather());

    emit started(n);
    qApp->processEvents();

   // auto progress=new QProgressDialog("Reading traces...","",0,query.size());
   // progress->show();
   // qApp->processEvents();

    while( query.next() && gather->size()<maxTraces){

        bool ok=false;
        int traceNo=query.value("trace").toInt(&ok);
        if( !ok){
            throw Exception(QString("Accessing trace number failed!"));
        }

        m_reader->seek_trace(traceNo);

        auto trc=m_reader->read_trace();

        addDData(trc);

        gather->push_back(trc);

        emit progress(gather->size());
        //progress->setValue(gather->size());
        //std::cout<<gather->size()<<" of "<<n<<std::endl<<std::flush;
        qApp->processEvents();
    }

    emit finished();
    //progress->close();
    qApp->processEvents();

    return gather;
}

/*
// only poststack for now, this works!
std::shared_ptr<seismic::Gather> SeismicDatasetReader::readGather( const QString& key1, const QString& key2,
                                                 QVector<std::pair<QString,QString>> values){

    emit started(values.size());
    qApp->processEvents();

    std::shared_ptr<seismic::Gather> gather(new seismic::Gather());
    QSqlQuery query("gather multiple keys", m_db);
    for( auto v12 : values){

        QString str=QString("select * from map where %1=%2 and %3=%4;").
                arg(key1, v12.first, key2, v12.second);
        //std::cout<<str.toStdString()<<std::endl;
        if( !query.exec(str)){
            throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
        }

        if( !query.next()) continue;
        bool ok=false;
        auto traceNo=query.value("trace").toInt(&ok);
        if( !ok){
            throw Exception(QString("Accessing trace number failed!"));
        }

        m_reader->seek_trace(traceNo);
        auto trc=m_reader->read_trace();
        //std::cout<<"> il="<<trc.header()["iline"]<<" xl="<<trc.header()["xline"]<<std::endl;
        addDData(trc);

        gather->push_back(trc);

        emit progress(gather->size());
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return gather;
}
*/

// for poststack data and integer keys only
std::shared_ptr<seismic::Gather> SeismicDatasetReader::readGather( const QString& key1, const QString& key2,
                                                 QVector<std::pair<QString,QString>> values){
    QString str=QString("select * from map where (%1,%2) in (values").arg(key1).arg(key2);
    for( int i=0; i<values.size(); i++){
        auto v12=values[i];
        auto s=QString("(%1,%2)").arg(v12.first).arg(v12.second);
        str+=s;
        if(i+1<values.size()) str+=",";
    }
    str+=");";

    // it is much faster to use a single query
    // the results have to be sorted to retain the original order
    QSqlQuery query("gather multiple keys", m_db);
    if( !query.exec(str)){
        throw Exception(QString("Querying index file failed: %1").arg(m_db.lastError().text()));
    }

    query.last();
    int n=query.at();
    query.first();

    emit started(values.size());
    qApp->processEvents();

    QVector<seismic::Trace> buf;
    QMap<QString,int> lookup;
    while( query.next() ){

        bool ok=false;
        int traceNo=query.value("trace").toInt(&ok);
        if( !ok){
            throw Exception(QString("Accessing trace number failed!"));
        }
        m_reader->seek_trace(traceNo);
        auto trc=m_reader->read_trace();
        addDData(trc);

        auto v1=trc.header()[key1.toStdString()].intValue();
        auto v2=trc.header()[key2.toStdString()].intValue();
        auto key=QString("%1_%2").arg(QString::number(v1),QString::number(v2));
        std::cout<<">>>"<<key.toStdString()<<" -> "<<buf.size()<<std::endl;
        lookup.insert( key, buf.size());
        buf.push_back(trc);

        emit progress(buf.size());
        qApp->processEvents();
    }

    // finally build gather in requested order
    auto gather=std::make_shared<seismic::Gather>();
    for(auto v12 : values){
        auto key=QString("%1_%2").arg(v12.first, v12.second);
        int idx=lookup.value(key,-1);
        if(idx>=0) gather->push_back(buf[idx]);
        std::cout<<"<<<"<<key.toStdString()<<" <- "<<idx<<std::endl;
    }

    emit finished();
    qApp->processEvents();

    return gather;
}

void SeismicDatasetReader::openDatabase(){

    static int counter=0;

    m_db = QSqlDatabase::addDatabase("QSQLITE", QString("%1-%2").arg(m_info.name()).arg(++counter) );
    // need to specify name otherwise default is used and multiple open datasets are mixed up

    m_db.setDatabaseName(m_info.indexPath());
    m_db.open();
    if( !m_db.isOpen()){
        throw Exception(QString("Open index file \"%1\" failed: %2").arg(m_info.indexPath(), m_db.lastError().text()));
    }
}


void SeismicDatasetReader::closeDatabase(){

    m_db.close();
}

void SeismicDatasetReader::openSEGYFile(){

    seismic::SEGYInfo segyInfo;
    QFile segyInfoFile(m_info.infoPath());
    if( !segyInfoFile.exists() || !segyInfoFile.open(QFile::ReadOnly | QFile::Text)){
        throw Exception( QString("Open segy info file \"%1\" failed!").arg(m_info.infoPath()));
    }
    seismic::XSIReader segyInfoReader(segyInfo);
    if( !segyInfoReader.read(&segyInfoFile)){
        throw Exception( QString("Reading segy info file \"%1\" failed!").arg(m_info.infoPath()));
    }

    m_reader=std::shared_ptr<seismic::SEGYReader>(new seismic::SEGYReader(m_info.path().toStdString(), segyInfo) );

    if( !m_reader ){
        throw Exception("Open segy reader failed!");
    }
}

void SeismicDatasetReader::closeSEGYFile(){

    if( m_reader) m_reader.reset();
}


