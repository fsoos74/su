#ifndef SECONDARYATTRIBUTEVOLUMESPROCESS_H
#define SECONDARYATTRIBUTEOLUMESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<functional>
#include<grid3d.h>

class SecondaryAttributeVolumesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    SecondaryAttributeVolumesProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    float GxI(int,int,int);
    float IqG(int,int,int);
    float GxIxFF(int,int,int);

    QString m_interceptName;
    QString m_gradientName;
    QString m_fluidFactorName;
    QString m_outputName;

    std::shared_ptr<Grid3D<float> > m_intercept;
    std::shared_ptr<Grid3D<float> > m_gradient;
    std::shared_ptr<Grid3D<float> > m_fluidFactor;
    std::shared_ptr<Grid3D<float> > m_volume;

    std::function<float(int, int, int)> m_func;
};

#endif // SECONDARYATTRBUTEVOLUMESPROCESS_H
