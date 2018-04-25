#ifndef VSHALEPROCESS_H
#define VSHALEPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>


class VShaleProcess : public ProjectProcess
{

    Q_OBJECT

public:

    enum class BlockingMode{ Curves, LayersTops};

    VShaleProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    void blockLogMin( Log& outputLog, const Log& inputLog, int i0, int i1);
    void blockLogMax( Log& outputLog, const Log& inputLog, int i0, int i1);
    ResultCode processWell(QString well);

    QStringList m_wells;
    QString m_grName;
    QString m_igrName;
    QString m_vshTertiaryRocksName;
    QString m_vshOlderRocksName;
    QString m_vshSteiberName;
    QString m_vshClavierName;
    QString m_grMinName;
    QString m_grMaxName;
    BlockingMode m_blockingMode;

    QStringList m_tops;
};

QString toQString(VShaleProcess::BlockingMode);
VShaleProcess::BlockingMode toBlockingMode(QString);


#endif // VSHALEPROCESS_H
