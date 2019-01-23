#ifndef EXPORTLASPROCESS_H
#define EXPORTLASPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<memory>
#include<log.h>

class ExportLASProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ExportLASProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_wellName;
    QStringList m_logNames;
    QString m_outputPath;
};

#endif // EXPORTLASPROCESS_H
