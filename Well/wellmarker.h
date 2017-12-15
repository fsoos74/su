#ifndef WELLMARKER_H
#define WELLMARKER_H

#include<QString>

class WellMarker
{
public:
    WellMarker(QString name="", QString uwi="", qreal md=0);

    QString name()const{
        return m_name;
    }

    QString uwi()const{
        return m_uwi;
    }

    qreal md()const{
        return m_md;
    }

    void setName(QString);
    void setMD(qreal);

private:
    QString m_name;
    QString m_uwi;
    qreal m_md;

};

#endif // WELLMARKER_H
