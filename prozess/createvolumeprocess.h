#ifndef CREATEVOLUMEPROCESS_H
#define CREATEVOLUMEPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<memory>


class CreateVolumeProcess : public VolumesProcess
{

    Q_OBJECT

public:

    CreateVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);

protected:
    virtual ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:
    double m_value;

};

#endif // CREATEVOLUMESPROCESS_H
