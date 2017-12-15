#ifndef CURVATUREVOLUMEPROCESS_H
#define CURVATUREVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>
#include<QSet>

class CurvatureVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CurvatureVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_baseName;
    int m_windowLength;
    int m_maximumShift;
    QSet<QString> m_output_attributes;
     QString m_volumeName;
    std::shared_ptr<Volume > m_volume;
};

#endif // CURVATUREVOLUMEPROCESS_H
