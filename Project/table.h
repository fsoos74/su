#ifndef TABLE_H
#define TABLE_H

#include <QMap>
#include <QList>
#include <tuple>


class Table
{

public:

    using key_t = qint32;
    using internal_key_t = qint64;
    using value_t = double;
    using const_iterator = QMap<internal_key_t, value_t>::const_iterator;

    Table(QString key1="iline", QString key2="xline", bool multi=false);

    int size()const{
        return m_data.size();
    }

    QString key1()const{
        return m_key1;
    }

    QString key2()const{
        return m_key2;
    }

    bool isMulti()const{
        return m_multi;
    }

    bool contains(key_t k1, key_t k2)const;
    bool contains(std::pair<key_t, key_t>)const;
    void insert(key_t k1, key_t k2, value_t v);
    void insert(std::pair<key_t, key_t>, value_t v);
    void insert(QList<std::tuple<key_t, key_t, value_t>>);
    value_t value(key_t k1, key_t k2)const;
    value_t value(std::pair<key_t,key_t>)const;
    QList<value_t> values(key_t k1, key_t k2)const;
    QList<value_t> values(std::pair<key_t,key_t>)const;
    QList<std::tuple<key_t, key_t, value_t>> atKey1(key_t k1)const;
    QList<std::tuple<key_t, key_t, value_t>> atKey2(key_t k2)const;
    QList<std::tuple<key_t, key_t, value_t>> inValueRange(value_t min, value_t max)const;   // // >=min and <max, [min,max)
    void remove(key_t k1, key_t k2);
    void remove(std::pair<key_t,key_t>);
    void removeValueRange(key_t k1, key_t k2, value_t min, value_t max);
    void removeValueRange(std::pair<key_t,key_t>, std::pair<value_t,value_t>);
    void clear();
    QList<std::pair<key_t,key_t>> keys()const;

    const_iterator cbegin()const{
        return m_data.cbegin();
    }

    const_iterator cend()const{
        return m_data.cend();
    }

    internal_key_t make_key(key_t k1, key_t k2)const{
        return (internal_key_t(k1)<<32) + k2;
    }

    std::pair<key_t, key_t> split_key(internal_key_t k)const{
        return std::make_pair( static_cast<key_t>(k>>32), static_cast<key_t>( k & 0xffffffff));
    }

private:

    QString m_key1;
    QString m_key2;
    bool m_multi;
    QMap<internal_key_t, value_t> m_data;
};

#endif // TABLE_H
