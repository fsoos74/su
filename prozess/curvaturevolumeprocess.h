#ifndef CURVATUREVOLUMEPROCESS_H
#define CURVATUREVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>

class CurvatureVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CurvatureVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;

    std::shared_ptr<Grid3D<float> > m_inputVolume;
    std::shared_ptr<Grid3D<float> > m_volume;
};

#endif // CURVATUREVOLUMEPROCESS_H
