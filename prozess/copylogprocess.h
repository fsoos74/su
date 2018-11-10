#ifndef COPYLOGPROCESS_H
#define COPYLOGPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>


class CopyLogProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CopyLogProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_sourceWell;
    QString m_sourceLog;
    QString m_destinationWell;
    QString m_destinationLog;
    QString m_description;
};

#endif // COPYLOGPROCESS_H
