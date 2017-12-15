#ifndef VOLUMEMATHPROCESS_H
#define VOLUMEMATHPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid2d.h>
#include<grid3d.h>
#include<memory>
#include<mathprocessor.h>


class VolumeMathProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumeMathProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    bool m_editMode;
    QString m_outputName;
    std::shared_ptr<Volume > m_inputVolume1;
    std::shared_ptr<Volume > m_inputVolume2;
    std::shared_ptr<Volume > m_outputVolume;
    std::shared_ptr<Grid2D<float>> m_topHorizon;
    std::shared_ptr<Grid2D<float>> m_bottomHorizon;
    MathProcessor m_processor;
};

#endif // VOLUMEMATHPROCESS_H
