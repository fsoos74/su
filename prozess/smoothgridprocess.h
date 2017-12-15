#ifndef SMOOTHGRIDPROCESS_H
#define SMOOTHGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<functional>

#include<grid2d.h>

#include <seismicdatasetreader.h>  // added this for qApp

class SmoothGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    enum class Method : int{ Mean, DistanceWeightedMean, TrimmedMean, Median };

    SmoothGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    void mean(int,int);
    void trimmedMean(int,int);
    void distanceWeightedMean(int,int);
    void median(int,int);

    GridType m_gridType;
    QString m_inputName;
    QString m_outputName;
    Method m_method;
    int m_halfIlines;
    int m_halfXlines;
    bool m_retainValues;

    std::shared_ptr<Grid2D<float> > m_inputGrid;
    std::shared_ptr<Grid2D<float> > m_outputGrid;

    std::function<void(int,int)> m_filter;
};


#endif // SMOOTHGRIDPTPROCESS_H
