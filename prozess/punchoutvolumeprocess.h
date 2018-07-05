#ifndef PUNCHOUTVOLUMEPROCESS_H
#define PUNCHOUTVOLUMEPROCESS_H

#include "projectprocess.h"
#include "volumesprocess.h"
#include<table.h>
#include<memory>


class PunchOutVolumeProcess : public VolumesProcess
{

    Q_OBJECT

public:

    PunchOutVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;

protected:
    ResultCode processInline(QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline)override;

private:

    QString m_tableName;
    bool m_keepPoints;

    std::shared_ptr<Table> m_table;
};

#endif // PUNCHOUTVOLUMEPROCESS_H
