#ifndef IMPORTGRIDPROCESS_H
#define IMPORTGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<grid2d.h>
#include<functional>
#include<QTransform>
#include<gridformat.h>

class ImportGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ImportGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    //void scan();
    bool processLine( const QString& line, int& il, int& xl, double& value);

    GridType m_type;
    QString m_name;
    QString m_fileName;
    GridFormat m_format;
    QString m_nullValue;
    int m_skipLines;
    int m_coord1Column;
    int m_coord2Column;
    int m_valueColumn;
    int m_lastUsedColumn;

    QTransform m_c1c2_to_ilxl;
    QTransform m_ilxl_to_c1c2;
};


#endif // EXPORTGRIDPPROCESS_H
