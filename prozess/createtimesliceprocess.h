#ifndef PROCESSCREATESLICE_H
#define PROCESSCREATESLICE_H

#include "projectprocess.h"
#include <seismicdatasetreader.h>
#include <grid2d.h>

class CreateTimesliceProcess : public ProjectProcess{

    Q_OBJECT

public:

    CreateTimesliceProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_sliceName;

    std::shared_ptr<SeismicDatasetReader> m_reader;
    std::shared_ptr<Grid2D<float> > m_horizon;
    std::shared_ptr<Grid2D<float> > m_slice;
    qreal m_sliceTime;
    bool m_useHorizon;
};



#endif // PROCESSCREATESLICE_H
