#ifndef RUNVOLUMESCRIPTPROCESS_H
#define RUNVOLUMESCRIPTPROCESS_H


#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>

#include<QVector>
#include<QTemporaryFile>
#include<grid2d.h>
#include<grid3d.h>

class RunVolumeScriptProcess : public ProjectProcess
{

    Q_OBJECT

public:

    RunVolumeScriptProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_volumeName;
    std::shared_ptr<Volume > m_volume;

    QVector<QString> m_inputVolumeName;
    QVector< std::shared_ptr<Volume > > m_inputVolume;

    QVector<QString> m_inputGridName;
    QVector< std::shared_ptr<Grid2D<float> > > m_inputGrid;

    QString m_script;

};


#endif // RUNVOLUMESCRIPTPROCESS_H
