#ifndef LOGMATHPROCESS_H
#define LOGMATHPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>
#include<functional>
#include<mathprocessor.h>


class LogMathProcess : public ProjectProcess
{

    Q_OBJECT

public:

    LogMathProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ProjectProcess::ResultCode processWell(QString well);

    float m_value=0;
    QStringList m_wells;
    QString m_outputName;
    QString m_unit;
    QString m_descr;
    QString m_inputName1;
    QString m_inputName2;

    MathProcessor m_processor;
};

#endif // LOGMATHPROCESS_H
