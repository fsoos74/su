#ifndef LOGINTEGRATIONPROCESS_H
#define LOGINTEGRATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>
#include<functional>
#include<integrationprocessor.h>


class LogIntegrationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    LogIntegrationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ProjectProcess::ResultCode processWell(QString well);

    QStringList m_wells;
    QString m_outputName;
    QString m_unit;
    QString m_descr;
    QString m_inputName;
    double m_startValue=0;

    IntegrationProcessor m_processor;
};

#endif // LOGINTEGRATIONPROCESS_H
