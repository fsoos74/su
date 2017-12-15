#ifndef UNFLATTENVOLUMEPROCESS_H
#define UNFLATTENVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>

class UnflattenVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    UnflattenVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
    QString m_horizonName;
    int m_start;
    int m_length;

    std::shared_ptr<Volume> m_inputVolume;
    std::shared_ptr<Grid2D<float>> m_horizon;
    std::shared_ptr<Volume> m_volume;
};

#endif
