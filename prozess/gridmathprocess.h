#ifndef GRIDMATHPROCESS_H
#define GRIDMATHPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<memory>
#include<functional>
#include<mathprocessor.h>


class GridMathProcess : public ProjectProcess
{

    Q_OBJECT

public:

    GridMathProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    float m_value=0;
    GridType m_outputType;
    QString m_outputName;
    std::shared_ptr<Grid2D<float> > m_inputGrid1;
    std::shared_ptr<Grid2D<float> > m_inputGrid2;
    std::shared_ptr<Grid2D<float> > m_outputGrid;
    MathProcessor m_processor;
};

#endif // GRIDMATHPROCESS_H
