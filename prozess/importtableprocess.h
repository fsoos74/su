#ifndef IMPORTTABLEPROCESS_H
#define IMPORTTABLEPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<table.h>
#include<functional>
#include<QTransform>
#include<gridformat.h>

class ImportTableProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ImportTableProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    //void scan();
    bool processLine( const QString& line, int& il, int& xl, double& value);

    QString m_name;
    QString m_fileName;
    GridFormat m_format;
    int m_skipLines;
    int m_coord1Column;
    int m_coord2Column;
    int m_valueColumn;
    int m_lastUsedColumn;

    QTransform m_c1c2_to_ilxl;
    QTransform m_ilxl_to_c1c2;
};


#endif // IMPORTTABLEPROCESS_H
