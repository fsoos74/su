#ifndef HORIZONCURVATUREPROCESS_H
#define HORIZONCURVATUREPROCESS_H

#include "projectprocess.h"
#include <QObject>
//#include <seismicdatasetreader.h>   // this gives qApp
#include<grid2d.h>
#include<memory>
#include <QSet>

#include <curvature_attributes.h> // names


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
