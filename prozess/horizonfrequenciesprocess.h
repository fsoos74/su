#ifndef HORIZONFREQUENCIESPROCESS_H
#define HORIZONFREQUENCIESPROCESS_H

#include "projectprocess.h"
#include "horizonwindowposition.h"
#include <QObject>

#include<grid2d.h>
#include<trace.h>
#include<fft.h>

class HorizonFrequenciesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    HorizonFrequenciesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:


    QString m_horizonName;
    QString m_gridName;
    size_t m_windowSamples;
    HorizonWindowPosition m_position;
    double m_minimumFrequency;
    double m_maximumFrequency;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    std::shared_ptr<Grid2D<float> > m_horizon;
    std::shared_ptr<Grid2D<float> > m_grid;

    //std::unique_ptr<ReductionFunction> m_reductionFunction;
};

#endif // HORIZONFREQUENCIESPROCESS_H
