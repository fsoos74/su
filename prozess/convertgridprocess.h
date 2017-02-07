#ifndef CONVERTGRIDPROCESS_H
#define CONVERTGRIDPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<grid2d.h>

#include <seismicdatasetreader.h>  // added this for qApp

class ConvertGridProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ConvertGridProcess( std::shared_ptr<AVOProject> project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    GridType m_inputType;
    QString m_inputName;

    GridType m_outputType;
    QString m_outputName;

    std::shared_ptr<Grid2D<float> > m_inputGrid;
    std::shared_ptr<Grid2D<float> > m_outputGrid;
};


#endif // CONVERTGRIDPTPROCESS_H
