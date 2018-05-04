#ifndef EXPORTTABLEPROCESS_H
#define EXPORTTABLEPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<functional>
#include<QTransform>
#include<gridformat.h>
#include<table.h>

class ExportTableProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ExportTableProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString zToQString(double);
    QString formatLineXYZ(int, int, double);
    QString formatLineILXLZ(int, int, double);
    QString formatLineXYILXLZ(int, int, double);

    std::shared_ptr<Table> m_inputTable;
    std::function<QString(int, int, double)> m_formatLineFunction;
    QString m_outputName;

    QTransform m_xy_to_ilxl, m_ilxl_to_xy;
};


#endif // EXPORTTABLEPPROCESS_H
