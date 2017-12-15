#ifndef EXPORTGRIDPROCESS_H
#define EXPORTGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<grid2d.h>
#include<functional>
#include<QTransform>
#include<gridformat.h>

class ExportGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ExportGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString zToQString(double);
    QString formatLineXYZ(int, int, double);
    QString formatLineILXLZ(int, int, double);
    QString formatLineXYILXLZ(int, int, double);

    std::shared_ptr<Grid2D<float> > m_inputGrid;
    std::function<QString(int, int, double)> m_formatLineFunction;
    QString m_null_value;
    QString m_outputName;

    QTransform m_xy_to_ilxl, m_ilxl_to_xy;
};


#endif // EXPORTGRIDPPROCESS_H
