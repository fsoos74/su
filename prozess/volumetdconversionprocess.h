#ifndef VOLUMETDCONVERSIONPROCESS_H
#define VOLUMETDCONVERSIONPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<memory>

enum class TDDirection{ TIME_TO_DEPTH, DEPTH_TO_TIME};
QString toQString(TDDirection);
TDDirection toTDDirection(QString);

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
    TDDirection m_direction;

};


#endif // VOLUMETDCONVERSIONPROCESS_H
