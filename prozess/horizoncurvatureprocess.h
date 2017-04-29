#ifndef HORIZONCURVATUREPROCESS_H
#define HORIZONCURVATUREPROCESS_H

#include "projectprocess.h"
#include <QObject>
//#include <seismicdatasetreader.h>   // this gives qApp
#include<grid2d.h>
#include<memory>
#include <QSet>

const QString MEAN_CURVATURE_STR("curv-mean");
const QString GAUSS_CURVATURE_STR("curv-gauss");
const QString MIN_CURVATURE_STR("curv-min");
const QString MAX_CURVATURE_STR("curv-max");
const QString MAX_POS_CURVATURE_STR("curv-max-pos");
const QString MAX_NEG_CURVATURE_STR("curv-max-neg");
const QString DIP_CURVATURE_STR("curv-dip");
const QString STRIKE_CURVATURE_STR("curv-strike");
const QString DIP_ANGLE_STR("dip-angle");
const QString DIP_AZIMUTH_STR("dip-azimuth");

class HorizonCurvatureProcess : public ProjectProcess
{

    Q_OBJECT

public:

    HorizonCurvatureProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:


    QString m_horizonName;
    QString m_baseName;
    QSet<QString> m_output_attributes;

    std::shared_ptr<Grid2D<float> > m_horizon;
};

#endif // HorizonCurvatureProcess_H
