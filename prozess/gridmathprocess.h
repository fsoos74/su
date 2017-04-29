#ifndef GRIDMATHPROCESS_H
#define GRIDMATHPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<memory>
#include<functional>

class GridMathProcess : public ProjectProcess
{

    Q_OBJECT

public:

    GridMathProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    float add_gv(int i, int j);
    float add_gg(int i, int j);
    float mul_gv(int i, int j);
    float mul_gg(int i, int j);

    float m_value=0;
    GridType m_outputType;
    QString m_outputName;
    std::shared_ptr<Grid2D<float> > m_inputGrid1;
    std::shared_ptr<Grid2D<float> > m_inputGrid2;
    std::shared_ptr<Grid2D<float> > m_outputGrid;

    std::function<float(int,int)> m_func;
};

#endif // GRIDMATHPROCESS_H
