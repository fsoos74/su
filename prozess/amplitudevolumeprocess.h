#ifndef AMPLITUDEVOLUMEPROCESS_H
#define AMPLITUDEVOLUMEPROCESS_H


#include<projectprocess.h>
#include<grid3d.h>


class AmplitudeVolumeProcess : public ProjectProcess
{
    Q_OBJECT

public:

    AmplitudeVolumeProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_volumeName;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    Grid3DBounds  m_bounds;
    std::shared_ptr<Grid3D<float> > m_volume;
};

#endif // AMPLITUDEVOLUMEPROCESS_H
