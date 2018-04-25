#ifndef EXPORTLOGPROCESS_H
#define EXPORTLOGPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<memory>
#include<log.h>

class ExportLogProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ExportLogProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_wellName;
    QStringList m_logNames;
    QString m_outputPath;
    QString m_nullStr;
    double m_zmin;
    double m_zmax;
};

#endif // EXPORTLOGPROCESS_H
