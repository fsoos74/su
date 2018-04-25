#include "topsdbmanager.h"
#include <stdexcept>

#include<QtSql>
#include<QSet>
#include <iostream>
#include<limits>

int TopsDBManager::m_counter=0;



TopsDBManager::TopsDBManager(const QString& path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE",QString("TopsDBManager-%1").arg(m_counter++));
    m_db.setDatabaseName(path);
    m_db.open();
    if (!m_db.isOpen())
    {
        throw std::runtime_error( QString("Open database %1 failed").arg(path).toStdString() );
    }

    QSqlQuery query(m_db);
    auto qstring = QString("create table if not exists %1 (name varchar(40), uwi varchar(20), md float, unique(name,uwi))").arg(TABLE);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

TopsDBManager::~TopsDBManager(){
    m_db.commit();
    m_db.close();
}

QStringList TopsDBManager::names(){

    QSqlQuery query("names", m_db);
    auto qstring = QString("select name from %1").arg(TABLE);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    QSet<QString> res;
    while( query.next()){
        res.insert(query.value(0).toString());
    }
    return QStringList::fromSet(res);
}

QStringList TopsDBManager::names(const QString& uwi){

    QSqlQuery query("names(well)", m_db);
    auto qstring = QString("select name from %1 where uwi='%2'").arg(TABLE, uwi);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    QSet<QString> res;
    while( query.next()){
        res.insert(query.value(0).toString());
    }
    return QStringList::fromSet(res);
}

QStringList TopsDBManager::wells(){

    QSqlQuery query("wells", m_db);
    auto qstring = QString("select uwi from %1").arg(TABLE);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    QSet<QString> res;
    while( query.next()){
        res.insert(query.value(0).toString());
    }
    return QStringList::fromSet(res);
}

QStringList TopsDBManager::wells(const QString& name){

    QSqlQuery query("wells(name)", m_db);
    auto qstring = QString("select uwi from %1 where name='%2'").arg(TABLE, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    QSet<QString> res;
    while( query.next()){
        res.insert(query.value(0).toString());
    }
    return QStringList::fromSet(res);
}

double TopsDBManager::value(const QString& uwi, const QString& name){

    QSqlQuery query("value", m_db);
    auto qstring = QString("select md from %1 where uwi='%2' and name='%3'").arg(TABLE, uwi, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    if( !query.next()) return 0; //quiet_NaN();

    return query.value(0).toDouble();
}


WellMarkers TopsDBManager::markers(){

    QSqlQuery query("markers", m_db);
    auto qstring = QString("select name,uwi,md from %1").arg(TABLE);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    WellMarkers res;
    while( query.next()){
        res.push_back(WellMarker( query.value(0).toString(), query.value(1).toString(), query.value(2).toDouble()));
    }

    return res;
}

WellMarkers TopsDBManager::markersByWell(const QString &uwi){

    QSqlQuery query("markersByWell", m_db);
    auto qstring = QString("select name,md from %1 where uwi='%2'").arg(TABLE, uwi);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    WellMarkers res;
    while( query.next()){
        res.push_back(WellMarker( query.value(0).toString(), uwi, query.value(1).toDouble()));
    }

    return res;
}

WellMarkers TopsDBManager::markersByName(const QString &name){

    QSqlQuery query("markersByName", m_db);
    auto qstring = QString("select uwi,md from %1 where name='%2'").arg(TABLE, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }

    WellMarkers res;
    while( query.next()){
        res.push_back(WellMarker( name, query.value(0).toString(), query.value(1).toDouble()));
    }

    return res;
}

bool TopsDBManager::exists(const QString &name){

    QSqlQuery query("exists(name)", m_db);
    auto qstring = QString("select * from %1 where name='%2'").arg(TABLE, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
    return query.next();
}

bool TopsDBManager::exists(const QString &uwi, const QString& name){

    QSqlQuery query("exists(uwi,name)", m_db);
    auto qstring = QString("select * from %1 where uwi='%2' and name='%3'").arg(TABLE, uwi, name);
    std::cout<<qstring.toStdString()<<std::endl<<std::flush;
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
    return query.next();
}

void TopsDBManager::clear(){

    QSqlQuery query("clear", m_db);
    auto qstring = QString("delete from %1;").arg(TABLE);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::remove(const QString &uwi, const QString &name){

    QSqlQuery query("remove", m_db);
    auto qstring = QString("delete from %1 where uwi = '%2' and name = '%3'").arg(TABLE, uwi, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::removeByName(const QString &name){

    QSqlQuery query("removeByName", m_db);
    auto qstring = QString("delete from %1 where name = '%2'").arg(TABLE, name);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::removeByWell(const QString &uwi){

    QSqlQuery query("removeByWell", m_db);
    auto qstring = QString("delete from %1 where uwi = '%2'").arg(TABLE, uwi);
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::insert(const WellMarker & wm){

    QSqlQuery query("insert", m_db);
    //auto qstring = QString("insert into %1 (name, uwi, md) VALUES ( '%2', '%3', %4)").arg(TABLE, wm.name(), wm.uwi(), QString::number(wm.md()));
    auto qstring = QString( "insert into %1 select '%2', '%3', %4 where not exists ( select 1 from %1 where name='%2' and uwi='%3')").arg(
                TABLE, wm.name(), wm.uwi(), QString::number(wm.md()));
    std::cout<<qstring.toStdString()<<std::endl<<std::flush;
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::replace(const WellMarker & wm){

    QSqlQuery query("replace", m_db);
    //auto qstring = QString("update %1 set md=%2 where name='%3' and uwi='%4'").arg(TABLE, QString::number(wm.md()), wm.name(), wm.uwi());
    auto qstring = QString("replace into %1 ( name, uwi, md) VALUES( '%2', '%3', %4)").arg(TABLE, wm.name(), wm.uwi(), QString::number(wm.md()));
    std::cout<<qstring.toStdString()<<std::endl<<std::flush;
    if( !query.exec(qstring)){
        throw std::runtime_error( query.lastError().text().toStdString());
    }
}

void TopsDBManager::set(const WellMarker & wm, bool replace){

    if( replace ){
        this->replace(wm);
    }
    else if( !exists(wm.uwi(), wm.name())){
        insert(wm);
    }
}
