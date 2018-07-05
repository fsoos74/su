#ifndef FLATTENVOLUMEPROCESS_H
#define FLATTENVOLUMEPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include <QObject>
#include<grid2d.h>
#include<memory>


class FlattenVolumeProcess : public VolumesProcess
{

    Q_OBJECT

public:

    enum class Mode{Flatten, Unflatten};
    static QString toQString(Mode);
    static Mode toMode(QString);

    FlattenVolumeProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(
            QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;
private:

    int m_start;
    int m_length;
    Mode m_mode;

    std::shared_ptr<Grid2D<float>> m_horizon;

};

#endif // SECONDARYATTRBUTEVOLUMESPROCESS_H
