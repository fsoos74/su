#ifndef VOLUMECIRCLEDETECTIONPROCESS_H
#define VOLUMECIRCLEDETECTIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include<memory>

class VolumeCircleDetectionProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumeCircleDetectionProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
    int m_minRadius;
    int m_maxRadius;
};

#endif //VOLUMECIRCLEDETECTIONPROCESS_H
