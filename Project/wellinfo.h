#ifndef WELLINFO_H
#define WELLINFO_H


#include<QString>


class WellInfo
{
public:
    WellInfo()=default;

    QString name()const{
        return m_name;
    }

    QString uwi()const{
        return m_uwi;
    }

    double x()const{
        return m_x;
    }

    double y()const{
        return m_y;
    }

    double z()const{
        return m_z;
    }

    void setName(const QString&);
    void setUWI(const QString&);
    void setX(double);
    void setY(double);
    void setZ(double);

private:
    QString m_name;
    QString m_uwi;
    double m_x=0;
    double m_y=0;
    double m_z=0;
};

#endif // WELLINFO_H
