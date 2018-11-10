#ifndef VOLUMEMATHPROCESS_H
#define VOLUMEMATHPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<grid2d.h>
#include<memory>
#include<mathprocessor.h>

class VolumeMathProcess : public VolumesProcess
{

    Q_OBJECT

public:

    VolumeMathProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

    std::shared_ptr<Grid2D<float>> m_topHorizon;
    std::shared_ptr<Grid2D<float>> m_bottomHorizon;
    MathProcessor m_processor;
};

#endif // VOLUMEMATHPROCESS_H
