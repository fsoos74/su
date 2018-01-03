#ifndef TOPSDBMANAGER_H
#define TOPSDBMANAGER_H

#include<QtSql>
#include<QStringList>
#include<wellmarker.h>
#include<wellmarkers.h>
#include<QVector>


class TopsDBManager
{
public:
    TopsDBManager( const QString& path );
    ~TopsDBManager();

    bool exists(const QString& name);
    bool exists(const QString& uwi, const QString& name);
    double value(const QString& uwi, const QString& name);

    QStringList names();
    QStringList names(const QString& uwi );
    QStringList wells();
    QStringList wells(const QString& name);

    WellMarkers markers();
    WellMarkers markersByWell(const QString& uwi);
    WellMarkers markersByName(const QString& name);

    void clear();
    void remove(const QString& uwi, const QString& name);
    void removeByName(const QString& name);
    void removeByWell(const QString& well);

    void insert(const WellMarker&);
    template<class ITER> inline void insert( ITER begin, ITER end);

    void replace(const WellMarker&);
    template<class ITER> inline void replace( ITER begin, ITER end);
    template<class ITER> inline void replaceAll( ITER begin, ITER end);

    void set(const WellMarker &, bool replace=true);

private:

    const QString TABLE="tops";

    static int m_counter;
    QSqlDatabase m_db;
};

template<class ITER>
inline void TopsDBManager::insert( ITER begin, ITER end){

    m_db.transaction();

    for( ITER it=begin; it!=end; ++it){
        insert(*it);
    }

    m_db.commit();
}


template<class ITER>
inline void TopsDBManager::replace( ITER begin, ITER end){

    m_db.transaction();

    for( ITER it=begin; it!=end; ++it){
        replace(*it);
    }

    m_db.commit();
}


template<class ITER>
inline void TopsDBManager::replaceAll( ITER begin, ITER end){

    m_db.transaction();

    clear();

    for( ITER it=begin; it!=end; ++it){
        insert(*it);
    }

    m_db.commit();
}


#endif // TOPSDBMANAGER_H
