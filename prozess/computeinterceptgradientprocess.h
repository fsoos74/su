#ifndef COMPUTEINTERCEPTGRADIENTPROCESS_H
#define COMPUTEINTERCEPTGRADIENTPROCESS_H

#include <QObject>

#include "projectprocess.h"
#include <seismicdatasetreader.h>
#include <grid2d.h>
#include <windowreductionfunction.h>
#include <memory>
#include "gatherbuffer.h"



class ComputeInterceptGradientProcess : public ProjectProcess
{
    Q_OBJECT

public:

    ComputeInterceptGradientProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    bool processBuffer_n(GatherBuffer*, int firstIline, int lastIline);


    QString m_horizonName;
    QString m_interceptName;
    QString m_gradientName;
    QString m_qualityName;
    size_t m_windowSize;
    double m_minimumOffset;
    double m_maximumOffset;
    double m_minimumAzimuth;
    double m_maximumAzimuth;
    ReductionMethod m_reductionMethod;
    bool m_supergatherMode=false;
    int m_supergatherInlineSize=1;
    int m_supergatherCrosslineSize=1;
    bool m_ava=false;
    double m_depth=0;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    std::shared_ptr<Grid2D<float> > m_horizon;
    std::unique_ptr<ReductionFunction> m_reductionFunction;
    std::shared_ptr<Grid2D<float> > m_intercept;
    std::shared_ptr<Grid2D<float> > m_gradient;
    std::shared_ptr<Grid2D<float> > m_quality;
};

#endif // COMPUTEINTERCEPTGRADIENTPROCESS_H
