#ifndef CREATEVOLUMEPROCESS_H
#define CREATEVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>
#include<log.h>
#include<QVector3D>

class CreateVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CreateVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_outputName;
    std::shared_ptr<Volume > m_volume;
};

#endif // CREATEVOLUMESPROCESS_H
