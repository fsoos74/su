#ifndef CASTAGNAPROCESS_H
#define CASTAGNAPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>


class CastagnaProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CastagnaProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString well);

    QStringList m_wells;
    QString m_vpName;
    QString m_vsName;
    QString m_description;
    double m_a0;
    double m_a1;
    double m_a2;
};



#endif // CASTAGNAPROCESS_H
