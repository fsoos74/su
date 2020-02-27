#ifndef IMPORTWELLSPROCESS_H
#define IMPORTWELLSPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <wellinfo.h>

class ImportWellsProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ImportWellsProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    bool processLine( const QString& line, WellInfo& );

    QString m_fileName;
    int m_skipLines;
    int m_nameColumn;
    int m_uwiColumn;
    int m_xColumn;
    int m_yColumn;
    int m_zColumn;
    int m_lastUsedColumn;
};


#endif // ImportWellsProcess_H
