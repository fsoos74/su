#ifndef CROPGRIDPROCESS_H
#define CROPGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>

class CropGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CropGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    GridType m_gridType;
    QString m_inputName;
    QString m_outputName;

    std::shared_ptr<Grid2D<float> > m_inputGrid;
    std::shared_ptr<Grid2D<float> > m_outputGrid;
};

#endif // CROPGRIDPROCESS_H
