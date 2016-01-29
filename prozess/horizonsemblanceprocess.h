#ifndef HORIZONSEMBLANCEPROCESS_H
#define HORIZONSEMBLANCEPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include <gather.h>
#include <trace.h>
#include <header.h>
#include<array2d.h>

using util::Array2D;

class HorizonSemblanceProcess : public ProjectProcess
{

    Q_OBJECT

public:

    HorizonSemblanceProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_horizonName;
    QString m_sliceName;
    size_t m_halfIlines;
    size_t m_halfXlines;
    size_t m_halfSamples;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    std::shared_ptr<Grid2D<double> > m_horizon;
    std::shared_ptr<Grid2D<double> > m_slice;

    std::shared_ptr< Array2D<seismic::Trace*> > traceBuffer=nullptr;
};

#endif // HORIZONSEMBLANCEPROCESS_H
