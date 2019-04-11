#ifndef MERGEVOLUMESPROCESS_H
#define MERGEVOLUMESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<functional>
#include<grid3d.h>

class MergeVolumesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    const int CHUNK_ILINES=10;

    MergeVolumesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ProjectProcess::ResultCode initFunction();

    float OV1(float, float);
    float OV2(float, float);
    float OMean(float, float);

    QString m_outputName;
    QString m_volume1Name;
    QString m_volume2Name;
    int m_minIline1;
    int m_maxIline1;
    int m_minIline2;
    int m_maxIline2;
    int m_ov;
    float m_null;

    VolumeReader2* m_volume1Reader;
    VolumeReader2* m_volume2Reader;
    Grid3DBounds m_bounds;
    std::function<float(float, float)> m_func;
};

#endif // MERGEVOLUMESPROCESS_H
