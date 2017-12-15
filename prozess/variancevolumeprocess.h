#ifndef VARIANCEVOLUMEPROCESS_H
#define VARIANCEVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include <gather.h>
#include <trace.h>
#include <header.h>
#include<array2d.h>

using util::Array2D;

class VarianceVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VarianceVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_volumeName;
    int m_halfIlines;
    int m_halfXlines;
    int m_halfSamples;

    std::shared_ptr<Volume > m_inputVolume;
    Grid3DBounds m_bounds;
    std::shared_ptr<Volume > m_volume;

    std::shared_ptr< Array2D<seismic::Trace*> > traceBuffer=nullptr;
};

#endif // VARIANCEVOLUMEPROCESS_H
