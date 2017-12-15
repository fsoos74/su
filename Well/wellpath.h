#ifndef WELLPATH_H
#define WELLPATH_H

#include "well_global.h"

#include <QVector>
#include <QVector3D>
#include <QPointF>
#include <log.h>

class WellPath{

public:

    typedef QVector<QVector3D>::const_iterator const_iterator;

    WellPath();
    WellPath( QVector<QVector3D> points);

    int size()const{
        return m_xyz.size();
    }

    const QVector3D* data()const{
        return m_xyz.data();
    }

    const QVector3D& operator[](int i)const{
        return m_xyz[i];
    }

    const QVector3D& at(int i)const{
        return m_xyz.at(i);
    }

    const_iterator cbegin()const{
        return m_xyz.cbegin();
    }

    const_iterator cend()const{
        return m_xyz.cend();
    }


    QPointF locationAtZ( qreal z)const;
    QPointF locationAtTVD( qreal tvd)const;
    QPointF locationAtMD( qreal md)const;
    qreal tvdAtMD( qreal md)const;
    qreal tvdAtZ( qreal z)const;
    qreal mdAtTVD( qreal tvd)const;
    qreal mdAtZ( qreal z)const;
    qreal zAtTVD( qreal tvd) const;
    qreal zAtMD( qreal md) const;

    static WellPath fromLog(const Log& log, qreal x=0, qreal y=0);   // create a pseudo vertical wellpath where tvd=md

private:

    void computeMD();
    int findLEByZ(qreal z)const;
    int findLEByMD(qreal md)const;

    QVector<QVector3D> m_xyz;
    QVector<qreal>     m_md;
};

#endif // WELLPATH_H
