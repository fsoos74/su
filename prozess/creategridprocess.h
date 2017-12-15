#ifndef CREATEGRIDPROCESS_H
#define CREATEGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<gridtype.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include<memory>

class CreateGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CreateGridProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    GridType m_outputType;
    QString m_outputName;
    std::shared_ptr<Grid2D<float> > m_grid;
};

#endif // CREATEGRIDPROCESS_H
