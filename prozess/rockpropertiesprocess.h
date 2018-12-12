#ifndef ROCKPROPERTIESPROCESS_H
#define ROCKPROPERTIESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>
#include<functional>


class RockPropertiesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    RockPropertiesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_well;
    QString m_dtName;
    QString m_dtsName;
    QString m_denName;
    QString m_youngsModulusName;
    QString m_bulkModulusName;
    QString m_shearModulusName;
    QString m_poissonRatioName;
};

#endif // ROCKPROPERTIESPROCESS_H
