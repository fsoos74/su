#ifndef TRENDBASEDATTRIBUTESPROCESS_H
#define TRENDBASEDATTRIBUTESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<functional>
#include<grid2d.h>

class TrendBasedAttributesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    TrendBasedAttributesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    void computeTrend();

    float FF(float, float);
    float LF(float, float);
    float PF(float, float);
    float AC(float, float);

    QString m_interceptName;
    QString m_gradientName;
    QString m_outputName;

    std::shared_ptr<Grid2D<float> > m_intercept;
    std::shared_ptr<Grid2D<float> > m_gradient;
    std::shared_ptr<Grid2D<float> > m_grid;

    float m_trendIntercept;
    float m_trendAngle;

    std::function<float(float, float)> m_func;

    float m_sinPhi;         // caching sin of trend_angle
    float m_cosPhi;         // caching cos of trend_angle
    float m_tanPhi;         // caching tan of trend_angle, == slope
};

#endif // TRENDBASEDATTRBUTESPROCESS_H
