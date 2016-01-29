#include "cdpdatabase.h"

#include<iostream>
#include<stdexcept>


CDPDatabase::CDPDatabase(const QString& fileName)
{
    connectToDatabase(fileName);
}


CDPDatabase::~CDPDatabase(){
    m_db.close();
}

void CDPDatabase::put( int cdp, const CDPData& data){

    QSqlQuery query;
    query.prepare("INSERT or REPLACE INTO cdps (cdp, iline, xline, x, y) "
                   "VALUES (:cdp, :iline, :xline, :x, :y)");
    query.bindValue(":cdp", cdp);
    query.bindValue(":iline", data.iline);
    query.bindValue(":xline", data.xline);
    query.bindValue(":x", data.x);
    query.bindValue(":y", data.y);

    if( !query.exec()) throw Exception(QString("CDPDatabase put: ")+query.lastError().text());
}

CDPDatabase::CDPData CDPDatabase::get( int cdp ){

    const int IndexIline=1;
    const int IndexXline=2;
    const int IndexX=3;
    const int IndexY=4;

    QSqlQuery query;
    query.prepare("SELECT * FROM cdps WHERE cdp=(:cdp)");
    query.bindValue(":cdp",cdp);
    if( !query.exec()) throw Exception(QString("CDPDatabase get: ")+query.lastError().text());


    if (!query.next()) throw Exception(QString("CDPDatabase get: next failed!"));

    CDPDatabase::CDPData data( query.record().value(IndexIline).toInt(),
                               query.record().value(IndexXline).toInt(),
                               query.record().value(IndexX).toDouble(),
                               query.record().value(IndexY).toDouble());

    return data;

}

void CDPDatabase::connectToDatabase(const QString& fileName){

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(fileName);
    m_db.open();

    QString queryStr = QString("CREATE TABLE IF NOT EXISTS %1 ("
            "cdp integer PRIMARY KEY,  iline  integer, xline  integer, x  real,  y real"
            ")").arg("cdps");
    QSqlQuery q(queryStr);

    if( !q.exec()) throw Exception(QString("CDPDatabase connect: ")+q.lastError().text());

}
