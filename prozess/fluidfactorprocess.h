#ifndef FLUIDFACTORPROCESS_H
#define FLUIDFACTORPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>

#include<grid2d.h>

class FluidFactorProcess : public ProjectProcess
{

    Q_OBJECT

public:

    FluidFactorProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_interceptName;
    QString m_gradientName;
    QString m_gridName;

    std::shared_ptr<Grid2D<double> > m_intercept;
    std::shared_ptr<Grid2D<double> > m_gradient;
    std::shared_ptr<Grid2D<double> > m_grid;
    bool m_computeAngle=true;
    double m_angle=0;             // radians
};

#endif // FLUIDFACTORPROCESS_H
