#include "table.h"

#include<iostream>


Table::Table(QString key1, QString key2, bool multi) :
    m_key1(key1), m_key2(key2), m_multi(multi)
{

}

void Table::clear(){
    m_data.clear();
}

bool Table::contains(key_t k1, key_t k2) const{
    auto k = make_key(k1,k2);
    return m_data.contains(k);
}

bool Table::contains(std::pair<key_t,key_t> k12) const{
    return contains(k12.first, k12.second);
}

void Table::insert(key_t k1, key_t k2, value_t v){
    auto k=make_key(k1,k2);
    if( m_multi){
        // allow multiple values per key but only once the same value
        if( m_data.contains(k) && m_data.values(k).contains(v) ) return;
        m_data.insertMulti(k,v);
    }
    else{
        m_data.insert(k,v);
    }
}

void Table::insert(std::pair<key_t,key_t> k12, value_t v){
    insert( k12.first, k12.second, v);
}

void Table::insert(QList<std::tuple<key_t, key_t, value_t>> keysAndValues){
    for( auto kkv : keysAndValues){
        insert(std::get<0>(kkv), std::get<1>(kkv), std::get<2>(kkv));
    }
}

Table::value_t Table::value(key_t k1, key_t k2) const{
    auto k= make_key(k1,k2);
    return m_data.value(k);     // silently ignore missing data
}

Table::value_t Table::value(std::pair<key_t,key_t> k12) const{
    return value(k12.first, k12.second);
}


QList<Table::value_t> Table::values(key_t k1, key_t k2) const{
    auto k=make_key(k1,k2);
    return m_data.values(k);    // silently ignore missing data
}

QList<Table::value_t> Table::values(std::pair<key_t,key_t> k12) const{
    return values(k12.first, k12.second);
}

QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> Table::atKey1(key_t k1)const{
    QList<std::tuple<key_t, key_t, value_t>> res;
    for(auto it = m_data.begin(); it!=m_data.end(); ++it){
        auto k12=split_key(it.key());
        if(k12.first==k1){
            res.push_back(std::make_tuple(k12.first, k12.second, it.value()) );
        }
    }

    return res;
}

QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> Table::atKey2(key_t k2)const{
    QList<std::tuple<key_t, key_t, value_t>> res;
    for(auto it = m_data.begin(); it!=m_data.end(); ++it){
        auto k12=split_key(it.key());
        if(k12.second==k2){
            res.push_back(std::make_tuple(k12.first, k12.second, it.value()) );
        }
    }

    return res;
}

// >=min and <max, [min,max)
QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> Table::inValueRange(value_t min, value_t max)const{
    QList<std::tuple<key_t, key_t, value_t>> res;
    for(auto it = m_data.begin(); it!=m_data.end(); ++it){
        if( it.value()>=min && it.value()<max){
            auto k12=split_key(it.key());
            res.push_back(std::make_tuple(k12.first, k12.second, it.value()) );
        }
    }

    return res;
}

void Table::remove(key_t k1, key_t k2){
    auto k=make_key(k1,k2);
    m_data.remove(k);
}

void Table::remove(std::pair<key_t,key_t> k12){
    remove(k12.first, k12.second);
}

void Table::removeValueRange(key_t k1, key_t k2, value_t min, value_t max){
    for(auto it = m_data.begin(); it!=m_data.end(); ++it){
        auto k12=split_key(it.key());
        if(k12.first!=k1 || k12.second!=k2) continue;
        if( it.value()>=min && it.value()<max){
            it=m_data.erase(it);        // returns iterator next to erased one, this keeps iterator/map valid
            if(it==m_data.end()) break;
        }
    }
}

void Table::removeValueRange(std::pair<key_t,key_t> k12, std::pair<value_t,value_t> minmax){
    removeValueRange(k12.first, k12.second, minmax.first, minmax.second);
}

QList<std::pair<Table::key_t,Table::key_t>> Table::keys()const{
    QList<std::pair<key_t,key_t>> res;
    for( auto k : m_data.keys()){
        res.push_back(split_key(k));
    }
    return res;
}
