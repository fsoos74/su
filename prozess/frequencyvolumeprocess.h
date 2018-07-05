#ifndef FREQUENCYVOLUMEPROCESS_H
#define FREQUENCYVOLUMEPROCESS_H

#include "projectprocess.h"
#include"volumesprocess.h"
#include <QObject>

class FrequencyVolumeProcess : public VolumesProcess
{
    Q_OBJECT

public:

    FrequencyVolumeProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

    size_t m_windowSamples;
    double m_minimumFrequency;
    double m_maximumFrequency;

};

#endif // FrequencyVolumeProcess_H
