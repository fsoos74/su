#ifndef PUNCHOUTVOLUMEPROCESS_H
#define PUNCHOUTVOLUMEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include<table.h>
#include<memory>
#include<QApplication>


class PunchOutVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:

    PunchOutVolumeProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
    QString m_tableName;
    bool m_keepPoints;

    std::shared_ptr<Volume > m_inputVolume;
    std::shared_ptr<Volume > m_volume;
    std::shared_ptr<Table> m_table;
};

#endif // SECONDARYATTRBUTEVOLUMESPROCESS_H
