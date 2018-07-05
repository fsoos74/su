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

    InterceptGradientVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processBuffer_n(GatherBuffer*, int il1, int il2);

    QString m_datasetName;
    QString m_interceptName;
    QString m_gradientName;
    QString m_qualityName;
    double m_minimumOffset;
    double m_maximumOffset;
    double m_minimumAzimuth;
    double m_maximumAzimuth;
    bool m_supergatherMode=false;
    int m_supergatherInlineSize=1;
    int m_supergatherCrosslineSize=1;
    bool m_timeWindow=false;
    int m_windowStart;
    int m_windowEnd;
    int m_startTraceSample=0;               // shift between trace samples and volume samples, since volume is created here dt are the same!

    std::shared_ptr<SeismicDatasetReader> m_reader;
#ifdef IO_VOLUMES
    VolumeWriter2* m_interceptWriter=nullptr;
    VolumeWriter2* m_gradientWriter=nullptr;
    VolumeWriter2* m_qualityWriter=nullptr;
#else
    std::shared_ptr<Volume > m_intercept;
    std::shared_ptr<Volume > m_gradient;
    std::shared_ptr<Volume > m_quality;
#endif
    Grid3DBounds  m_bounds;

};

#endif // INTERCEPTGRADIENTVOLUMEPROCESS_H
