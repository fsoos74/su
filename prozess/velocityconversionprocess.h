#ifndef VELOCITYCONVERSIONPROCESS_H
#define VELOCITYCONVERSIONPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<memory>


class VelocityConversionProcess : public VolumesProcess
{

    Q_OBJECT

public:

    enum Conversion{NoConversion,IntervalToAverage, AverageToInterval};

    VelocityConversionProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

    Conversion m_ctype;
};


#endif // VELOCITYCONVERSIONPROCESS_H
