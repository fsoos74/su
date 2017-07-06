#ifndef INSTANTANEOUSATTRIBUTESPROCESS_H
#define INSTANTANEOUSATTRIBUTESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>
#include<QSet>

class InstantaneousAttributesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    InstantaneousAttributesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    std::shared_ptr<Grid3D<float> > m_inputVolume;

    QString m_amplitudeVolumeName;
    QString m_phaseVolumeName;
    QString m_frequencyVolumeName;
};

#endif // InstantaneousAttributesProcess_H
