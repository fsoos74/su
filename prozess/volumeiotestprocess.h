#ifndef VOLUMEIOTESTPROCESS_H
#define VOLUMEIOTESTPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include<memory>

class VolumeIOTestProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumeIOTestProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
};

#endif //VOLUMEIOTESTPROCESS_H
