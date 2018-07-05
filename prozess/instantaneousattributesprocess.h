#ifndef INSTANTANEOUSATTRIBUTESPROCESS_H
#define INSTANTANEOUSATTRIBUTESPROCESS_H

#include "volumesprocess.h"
#include <QObject>


class InstantaneousAttributesProcess : public VolumesProcess
{
    Q_OBJECT

public:

    InstantaneousAttributesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(QVector<std::shared_ptr<Volume> > outputs,
                        QVector<std::shared_ptr<Volume> > inputs, int iline)override;
};

#endif // InstantaneousAttributesProcess_H
