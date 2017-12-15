#ifndef IMPORTMARKERSPROCESS_H
#define IMPORTMARKERSPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <QMap>

#include<wellmarkers.h>



class ImportMarkersProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ImportMarkersProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    bool processLine( const QString& line, QString& uwi, QString& name, double& md);

    QString m_fileName;
    QString m_separator;
    int m_skipLines;
    int m_uwiColumn;
    int m_nameColumn;
    int m_mdColumn;
    int m_lastUsedColumn;
    bool m_replace;
};


#endif // IMPORTMARKERSPPROCESS_H
