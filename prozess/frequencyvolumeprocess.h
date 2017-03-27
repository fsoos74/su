#ifndef FREQUENCYVOLUMEPROCESS_H
#define FREQUENCYVOLUMEPROCESS_H


#include<projectprocess.h>
#include<grid3d.h>
#include<horizonwindowposition.h>

class FrequencyVolumeProcess : public ProjectProcess
{
    Q_OBJECT

public:

    FrequencyVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    size_t m_windowSamples;
    HorizonWindowPosition m_position;
    double m_minimumFrequency;
    double m_maximumFrequency;
    QString m_volumeName;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    Grid3DBounds  m_bounds;
    std::shared_ptr<Grid3D<float> > m_volume;
};

#endif // FrequencyVolumeProcess_H
