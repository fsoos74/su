#ifndef VOLUMESTATISTICSPROCESS_H
#define VOLUMESTATISTICSPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include <gather.h>
#include <trace.h>
#include <header.h>
#include<array2d.h>
#include<functional>
#include<volumediptracker.h>



using util::Array2D;

class VolumeStatisticsProcess : public ProjectProcess
{

    Q_OBJECT

public:

    //enum StatType{ MIN, MAX, MEAN, TRIMMED_MEAN, VARIANCE, STANDARD_DEVIATION, MEDIAN };

    VolumeStatisticsProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    void fillBuffer( std::vector<float>& buffer, int i, int j, int k);
    void fillBuffer_tracked( std::vector<float>& buffer, int i, int j, int k);

    QString m_volumeName;
    int m_halfIlines;
    int m_halfXlines;
    int m_halfSamples;

    std::shared_ptr<Volume > m_inputVolume;
    bool m_track;
    std::shared_ptr<VolumeDipTracker> m_tracker;
    Grid3DBounds m_bounds;
    std::shared_ptr<Volume > m_volume;

    std::function<float(float*,size_t)> m_func;
};

#endif // VOLUMESTATISTICSPROCESS_H
