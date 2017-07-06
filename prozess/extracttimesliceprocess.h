#ifndef EXTRACTTIMESLICEPROCESS_H
#define EXTRACTTIMESLICEPROCESS_H

#include "projectprocess.h"
#include <seismicdatasetreader.h>
#include <grid2d.h>
#include <grid3d.h>

class ExtractTimesliceProcess : public ProjectProcess{

    Q_OBJECT

public:

    ExtractTimesliceProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_sliceName;
    std::shared_ptr<Grid3D<float> > m_volume;
    std::shared_ptr<Grid2D<float> > m_horizon;
    std::shared_ptr<Grid2D<float> > m_slice;
    qreal m_sliceTime;
};



#endif // PROCESSCREATESLICE_H
