#ifndef BUILDWELLVOLUMEPROCESS_H
#define BUILDWELLVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>
#include<log.h>
#include<QVector3D>

class BuildWellVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    BuildWellVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_outputName;
    QString m_logName;
    QStringList m_wellNames;
    QVector<std::shared_ptr<Log>> m_logs;
    QVector<std::shared_ptr<WellPath>> m_paths;
    std::shared_ptr<Grid2D<float>> m_topHorizon;
    std::shared_ptr<Grid2D<float>> m_bottomHorizon;
    int m_filterLen=0;
    std::shared_ptr<Volume > m_volume;
};

#endif // BUILDWELLVOLUMESPROCESS_H
