#ifndef LOGSCRIPTPROCESS_H
#define LOGSCRIPTPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>

#include<QVector>
#include<QTemporaryFile>
#include<log.h>


class LogScriptProcess : public ProjectProcess
{

    Q_OBJECT

public:

    LogScriptProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ProjectProcess::ResultCode initPython();
    void finishPython();
    ProjectProcess::ResultCode initWell(QString);
    ProjectProcess::ResultCode processWell(QString);

    QString m_outputName;
    QString m_unit;
    QString m_descr;
    QStringList m_wells;
    QStringList m_inputNames;
    QString m_script;

    QVector<std::shared_ptr<Log>> m_inputLogs;
    std::shared_ptr<Log> m_log;
};


#endif // LOGSCRIPTPROCESS_H
