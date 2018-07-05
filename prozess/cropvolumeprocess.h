#ifndef CROPVOLUMEPROCESS_H
#define CROPVOLUMEPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"


class CropVolumeProcess : public VolumesProcess
{

    Q_OBJECT

public:

    CropVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

};

#endif // CROPVOLUMEPROCESS_H
