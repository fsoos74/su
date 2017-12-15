#ifndef SMOOTHLOGPROCESS_H
#define SMOOTHLOGPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>
#include<smoothprocessor.h>


class SmoothLogProcess : public ProjectProcess
{

    Q_OBJECT

public:

    SmoothLogProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString);

    QStringList m_wells;
    QString m_outputName;
    QString m_unit;
    QString m_descr;
    QString m_inputName;
    int m_aperture;

    SmoothProcessor m_processor;
};

#endif // SMOOTHLOGPROCESS_H
