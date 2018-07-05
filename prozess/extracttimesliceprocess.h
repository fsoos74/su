#ifndef EXTRACTTIMESLICEPROCESS_H
#define EXTRACTTIMESLICEPROCESS_H

#include "volumesprocess.h"
#include <grid2d.h>
#include <grid3d.h>
#include <memory>


class ExtractTimesliceProcess : public VolumesProcess{

    Q_OBJECT

public:

    ExtractTimesliceProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;
    ResultCode finalize()override;

private:
    QString m_sliceName;
    bool m_useHorizon;
    qreal m_sliceTime;
    std::shared_ptr<Grid2D<float> > m_horizon;
    std::shared_ptr<Grid2D<float> > m_slice;
};



#endif // PROCESSCREATESLICE_H
