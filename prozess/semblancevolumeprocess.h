#ifndef SEMBLANCEVOLUMEPROCESS_H
#define SEMBLANCEVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include <gather.h>
#include <trace.h>
#include <header.h>
#include<array2d.h>

using util::Array2D;

class SemblanceVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    SemblanceVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_volumeName;
    size_t m_halfIlines;
    size_t m_halfXlines;
    size_t m_halfSamples;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    Grid3DBounds m_bounds;
    std::shared_ptr<Grid3D<float> > m_volume;

    std::shared_ptr< Array2D<seismic::Trace*> > traceBuffer=nullptr;
};

#endif // SEMBLANCEVOLUMEPROCESS_H
