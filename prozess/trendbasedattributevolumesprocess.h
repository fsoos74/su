#ifndef TRENDBASEDATTRIBUTEVOLUMESPROCESS_H
#define TRENDBASEDATTRIBUTEVOLUMESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<functional>
#include<grid3d.h>

class TrendBasedAttributeVolumesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    const int CHUNK_ILINES=10;

    TrendBasedAttributeVolumesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode computeTrend();
    ResultCode initFunction();

    float FF(float, float);
    float LF(float, float);
    float PF(float, float);
    float AC(float, float);

    QString m_outputName;

    QString m_interceptName;
    QString m_gradientName;

    bool m_computeTrend;
    float m_trendIntercept;
    float m_trendAngle;

    int m_attribute;

    std::shared_ptr<VolumeReader2 > m_interceptReader;
    std::shared_ptr<VolumeReader2 > m_gradientReader;
    Grid3DBounds m_bounds;
    std::function<float(float, float)> m_func;

    float m_sinPhi;         // caching sin of trend_angle
    float m_cosPhi;         // caching cos of trend_angle
    float m_tanPhi;         // caching tan of trend_angle, == slope
};

#endif // TRENDBASEDATTRBUTEVOLUMESPROCESS_H
