#ifndef TABLETOHORIZONPROCESS_H
#define TABLETOHORIZONPPROCESS_H

#include "projectprocess.h"
#include <QObject>


class TableToHorizonProcess : public ProjectProcess
{

    Q_OBJECT

public:

    TableToHorizonProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString well);

    QString m_horizonName;
    QString m_tableName;
};


#endif // TABLETOPPROCESS_H
