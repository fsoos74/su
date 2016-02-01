#ifndef INTERCEPTGRADIENTVOLUMEPROCESS_H
#define INTERCEPTGRADIENTVOLUMEPROCESS_H

#include <QObject>

#include "projectprocess.h"
#include <seismicdatasetreader.h>
#include <grid3d.h>
#include <windowreductionfunction.h>
#include <memory>
#include "gatherbuffer.h"



class InterceptGradientVolumeProcess : public ProjectProcess
{
    Q_OBJECT

public:

    InterceptGradientVolumeProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    bool processBuffer_n(GatherBuffer*, int firstIline, int lastIline);

    QString m_interceptName;
    QString m_gradientName;
    double m_maximumOffset;
    double m_minimumAzimuth;
    double m_maximumAzimuth;
    bool m_supergatherMode=false;
    int m_supergatherInlineSize=1;
    int m_supergatherCrosslineSize=1;
    int m_startTraceSample=0;               // shift between trace samples and volume samples, since volume is created here dt are the same!

    std::shared_ptr<SeismicDatasetReader> m_reader;
    Grid3DBounds  m_bounds;
    std::shared_ptr<Grid3D<float> > m_intercept;
    std::shared_ptr<Grid3D<float> > m_gradient;

};

#endif // INTERCEPTGRADIENTVOLUMEPROCESS_H
