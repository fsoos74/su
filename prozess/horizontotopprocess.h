#ifndef HORIZONTOTOPPROCESS_H
#define HORIZONTOTOPPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid.h>
#include<memory>
#include<QTransform>


class HorizonToTopProcess : public ProjectProcess
{

    Q_OBJECT

public:

    HorizonToTopProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString well);

    QStringList m_wells;
    QString m_horizonName;
    QString m_topName;
    std::shared_ptr<Grid2D<float>> m_horizon;
    QTransform xy_to_ilxl, ilxl_to_xy;
};


#endif // HORIZONTOTOPPROCESS_H
