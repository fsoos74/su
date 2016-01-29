#ifndef HORIZONAMPLITUDESPROCESS_H
#define HORIZONAMPLITUDESPROCESS_H

#include "projectprocess.h"
#include "windowreductionfunction.h"
#include <QObject>

#include<grid2d.h>
#include<trace.h>

class HorizonAmplitudesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    HorizonAmplitudesProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:


    QString m_horizonName;
    QString m_gridName;
    size_t m_halfSamples;
    ReductionMethod m_method;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    std::shared_ptr<Grid2D<double> > m_horizon;
    std::shared_ptr<Grid2D<double> > m_grid;

    std::unique_ptr<ReductionFunction> m_reductionFunction;
};

#endif // HORIZONAMPLITUDESPROCESS_H
