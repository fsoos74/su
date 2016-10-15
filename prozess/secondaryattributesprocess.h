#ifndef SECONDARYATTRIBUTESPROCESS_H
#define SECONDARYATTRIBUTESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include<functional>
#include<grid2d.h>

class SecondaryAttributesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    SecondaryAttributesProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    float GxI(int,int);
    float IqG(int,int);
    float GxIxFF(int,int);

    QString m_interceptName;
    QString m_gradientName;
    QString m_fluidFactorName;
    QString m_outputName;

    std::shared_ptr<Grid2D<float> > m_intercept;
    std::shared_ptr<Grid2D<float> > m_gradient;
    std::shared_ptr<Grid2D<float> > m_fluidFactor;
    std::shared_ptr<Grid2D<float> > m_grid;

    std::function<float(int, int)> m_func;
};

#endif // SECONDARYATTRBUTESPROCESS_H
