#ifndef SMOOTHGRIDPROCESS_H
#define SMOOTHGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<grid2d.h>

#include <seismicdatasetreader.h>  // added this for qApp

class SmoothGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    SmoothGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    GridType m_gridType;
    QString m_inputName;
    QString m_outputName;
    int m_halfIlines;
    int m_halfXlines;

    std::shared_ptr<Grid2D<float> > m_inputGrid;
    std::shared_ptr<Grid2D<float> > m_outputGrid;
};


#endif // SMOOTHGRIDPTPROCESS_H
