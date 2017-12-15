#ifndef FLUIDFACTORVOLUMEPROCESS_H
#define FLUIDFACTORVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<grid3d.h>

class FluidFactorVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    FluidFactorVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_interceptName;
    QString m_gradientName;
    QString m_volumeName;

    std::shared_ptr<Volume > m_intercept;
    std::shared_ptr<Volume > m_gradient;
    std::shared_ptr<Volume > m_volume;

    bool m_computeAngle=true;
    double m_angle=0;             // radians
};

#endif // FLUIDFACTORVOLUMEPROCESS_H
