#ifndef FLATTENVOLUMEPROCESS_H
#define FLATTENVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>

class FlattenVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    FlattenVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
    QString m_horizonName;
    int m_length;

    std::shared_ptr<Volume> m_inputVolume;
    std::shared_ptr<Grid2D<float>> m_horizon;
    std::shared_ptr<Volume> m_volume;
};

#endif // SECONDARYATTRBUTEVOLUMESPROCESS_H
