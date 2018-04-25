#ifndef VOLUMEDIPSCANPROCESS_H
#define VOLUMEDIPSCANPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include<memory>
#include<functional>

class VolumeDipScanProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumeDipScanProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    void scan_maximum_semblance(int,int,int);

    QString m_ilineDipName;
    QString m_xlineDipName;
    QString m_semblanceName;

    std::shared_ptr<Volume > m_inputVolume;
    std::shared_ptr<Volume > m_ilineDipVolume;
    std::shared_ptr<Volume > m_xlineDipVolume;
    std::shared_ptr<Volume > m_semblanceVolume;

    int m_windowLines;
    int m_windowSamples;
    double m_minimumDip;
    double m_maximumDip;
    int m_dipCount;
};

#endif // VOLUMEDIPPROCESS_H
