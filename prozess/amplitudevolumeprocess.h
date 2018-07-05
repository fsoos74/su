#ifndef AMPLITUDEVOLUMEPROCESS_H
#define AMPLITUDEVOLUMEPROCESS_H


#include<projectprocess.h>
#include<grid3d.h>


class AmplitudeVolumeProcess : public ProjectProcess
{
    Q_OBJECT

public:

    AmplitudeVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_volumeName;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    Grid3DBounds  m_bounds;
#ifdef IO_VOLUMES
    const int CHUNK_SIZE=10;
    VolumeWriter2* m_volumeWriter;
#else
    std::shared_ptr<Volume > m_volume;
#endif
};

#endif // AMPLITUDEVOLUMEPROCESS_H
