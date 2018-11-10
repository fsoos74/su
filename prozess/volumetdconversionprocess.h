#ifndef VOLUMETDCONVERSIONPROCESS_H
#define VOLUMETDCONVERSIONPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<memory>

class VolumeTDConversionProcess : public VolumesProcess
{

    Q_OBJECT

public:

    VolumeTDConversionProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

};

#endif // VOLUMETDCONVERSIONPROCESS_H
